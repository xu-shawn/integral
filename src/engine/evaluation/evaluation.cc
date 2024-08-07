#include "evaluation.h"

#include "../../tuner/tuner.h"

namespace eval {

namespace masks {

constexpr SideTable<SquareTable<BitBoard>> GenerateForwardFiles() {
  SideTable<SquareTable<BitBoard>> forward_file;
  for (Square square = 0; square < kSquareCount; square++) {
    forward_file[Color::kWhite][square] =
        ForwardFileMask(Color::kWhite, square);
    forward_file[Color::kBlack][square] =
        ForwardFileMask(Color::kBlack, square);
  }
  return forward_file;
}

constexpr SideTable<SquareTable<BitBoard>> forward_file =
    GenerateForwardFiles();

constexpr SideTable<SquareTable<BitBoard>> GenerateForwardFileAdjacent() {
  SideTable<SquareTable<BitBoard>> forward_file_adjacent;
  for (Square square = 0; square < kSquareCount; square++) {
    const BitBoard white_forward = forward_file[Color::kWhite][square];
    forward_file_adjacent[Color::kWhite][square] =
        Shift<Direction::kWest>(white_forward) | white_forward |
        Shift<Direction::kEast>(white_forward);

    const BitBoard black_forward = forward_file[Color::kBlack][square];
    forward_file_adjacent[Color::kBlack][square] =
        Shift<Direction::kWest>(black_forward) | black_forward |
        Shift<Direction::kEast>(black_forward);
  }
  return forward_file_adjacent;
}

constexpr SideTable<SquareTable<BitBoard>> forward_file_adjacent =
    GenerateForwardFileAdjacent();

constexpr SquareTable<BitBoard> GenerateFiles() {
  SquareTable<BitBoard> files;
  for (Square square = 0; square < kSquareCount; square++) {
    files[square] = kFileMasks[square.File()];
  }
  return files;
}

constexpr SquareTable<BitBoard> files = GenerateFiles();

constexpr SquareTable<BitBoard> GenerateAdjacentFiles() {
  SquareTable<BitBoard> adjacent_files;
  for (Square square = 0; square < kSquareCount; square++) {
    adjacent_files[square] = Shift<Direction::kWest>(files[square]) |
                             Shift<Direction::kEast>(files[square]);
  }
  return adjacent_files;
}

constexpr SquareTable<BitBoard> adjacent_files = GenerateAdjacentFiles();

constexpr SideTable<BitBoard> GenerateOutposts() {
  SideTable<BitBoard> outposts;

  outposts.fill(kRankMasks[kRank4] | kRankMasks[kRank5]);
  outposts[Color::kWhite] |= kRankMasks[kRank6];
  outposts[Color::kBlack] |= kRankMasks[kRank3];

  return outposts;
}

constexpr SideTable<BitBoard> outposts = GenerateOutposts();

}  // namespace masks

class Evaluation {
 public:
  explicit Evaluation(const BoardState &state) : state_(state) {
    Initialize();
  }

  Score GetScore();

 private:
  void Initialize();

  template <Color us>
  ScorePair EvaluatePawns();

  template <Color us>
  ScorePair EvaluateKnights();

  template <Color us>
  ScorePair EvaluateBishops();

  template <Color us>
  ScorePair EvaluateRooks();

  template <Color us>
  ScorePair EvaluateQueens();

  template <Color us>
  ScorePair EvaluateKing();

  template <Color us>
  ScorePair EvaluateThreats();

  [[nodiscard]] BitBoard LegalizeMoves(PieceType piece,
                                       Square square,
                                       BitBoard moves,
                                       Color us) const;

  [[nodiscard]] Score InterpolateScore(ScorePair score_pair) const;

  template <Color us>
  [[nodiscard]] bool IsDefendedByPawn(Square square) const;

  template <Color us>
  [[nodiscard]] bool IsOutpostSquare(Square square) const;

 private:
  const BoardState &state_;
  SideTable<BitBoard> king_zone_;
  SideTable<BitBoard> pawn_attacks_;
  SideTable<BitBoard> knight_attacks_;
  SideTable<BitBoard> bishop_attacks_;
  SideTable<BitBoard> rook_attacks_;
  SideTable<BitBoard> queen_attacks_;
  SideTable<BitBoard> mobility_zone_;
  SideTable<BitBoard> pawn_storm_zone_;
  SideTable<ScorePair> attack_power_;
  PawnStructureEntry *cached_pawn_structure_;
  bool has_pawn_structure_cache_;
};

void Evaluation::Initialize() {
  const Square white_king_square = state_.King(Color::kWhite).GetLsb();
  const Square black_king_square = state_.King(Color::kBlack).GetLsb();

  auto &white_king_zone = king_zone_[Color::kWhite];
  auto &black_king_zone = king_zone_[Color::kBlack];

  // Create the 3x3 zone around the king
  white_king_zone = move_gen::KingAttacks(white_king_square);
  black_king_zone = move_gen::KingAttacks(black_king_square);

  // Add the extra rank in front of the king
  white_king_zone |= Shift<Direction::kNorth>(white_king_zone);
  black_king_zone |= Shift<Direction::kSouth>(black_king_zone);

  pawn_attacks_[Color::kWhite] =
      move_gen::PawnAttacks(state_.Pawns(Color::kWhite), Color::kWhite);
  pawn_attacks_[Color::kBlack] =
      move_gen::PawnAttacks(state_.Pawns(Color::kBlack), Color::kBlack);

  // Remove enemy pawn attacks from the mobility zone
  mobility_zone_[Color::kWhite] =
      ~(state_.Occupied(Color::kWhite) | pawn_attacks_[Color::kBlack]);
  mobility_zone_[Color::kBlack] =
      ~(state_.Occupied(Color::kBlack) | pawn_attacks_[Color::kWhite]);

  // King's + adjacent files forward from the king
  auto &white_pawn_storm_zone = pawn_storm_zone_[Color::kWhite];
  auto &black_pawn_storm_zone = pawn_storm_zone_[Color::kBlack];

  white_pawn_storm_zone =
      masks::forward_file_adjacent[Color::kWhite][white_king_square];
  black_pawn_storm_zone =
      masks::forward_file_adjacent[Color::kBlack][black_king_square];

  // Add in the squares next to the king
  white_pawn_storm_zone |= Shift<Direction::kSouth>(white_pawn_storm_zone);
  black_pawn_storm_zone |= Shift<Direction::kNorth>(black_pawn_storm_zone);

  // Probe the pawn structure cache
  cached_pawn_structure_ = &pawn_cache[state_.pawn_key];
  has_pawn_structure_cache_ = cached_pawn_structure_->key == state_.pawn_key;
}

Score Evaluation::GetScore() {
  const Color us = state_.turn;

  ScorePair score =
      state_.piece_scores[Color::kWhite] - state_.piece_scores[Color::kBlack];

  score += EvaluatePawns<Color::kWhite>() - EvaluatePawns<Color::kBlack>();
  score += EvaluateKnights<Color::kWhite>() - EvaluateKnights<Color::kBlack>();
  score += EvaluateBishops<Color::kWhite>() - EvaluateBishops<Color::kBlack>();
  score += EvaluateRooks<Color::kWhite>() - EvaluateRooks<Color::kBlack>();
  score += EvaluateQueens<Color::kWhite>() - EvaluateQueens<Color::kBlack>();
  score += EvaluateKing<Color::kWhite>() - EvaluateKing<Color::kBlack>();
  score += EvaluateThreats<Color::kWhite>() - EvaluateThreats<Color::kBlack>();

  // Flip the score if we are black, since the score is from white's perspective
  if (us == Color::kBlack) {
    score *= -1;
  }

  // A constant bonus given to the side to move
  score += kTempoBonus;
  TRACE_INCREMENT(kTempoBonus, us);

  const Score final_score = InterpolateScore(score);

  TRACE_EVAL(final_score);
  return final_score;
}

template <Color us>
ScorePair Evaluation::EvaluatePawns() {
  ScorePair score;

  const Color them = FlipColor(us);

  const BitBoard our_pawns = state_.Pawns(us);
  const BitBoard their_pawns = state_.Pawns(them);

  BitBoard passed_pawns;

  if (!has_pawn_structure_cache_) {
    // Pawn phalanxes
    const BitBoard connected_pawns =
        Shift<Direction::kEast>(our_pawns) & our_pawns;
    for (Square square : connected_pawns) {
      score += kPawnPhalanxBonus[square.RelativeRank<us>()];
      TRACE_INCREMENT(kPawnPhalanxBonus[square.RelativeRank<us>()], us);
    }

    for (Square square : our_pawns) {
      TRACE_INCREMENT(kPieceValues[kPawn], us);
      TRACE_INCREMENT(kPieceSquareTable[kPawn][square.RelativeTo(us)], us);

      const int file = square.File();
      const int rank = square.RelativeRank<us>();

      // Passed pawns
      const BitBoard their_pawns_ahead =
          masks::forward_file_adjacent[us][square] & their_pawns;
      if (!their_pawns_ahead) {
        passed_pawns.SetBit(square);

        score += kPassedPawnBonus[rank];
        TRACE_INCREMENT(kPassedPawnBonus[rank], us);
      }

      if (IsDefendedByPawn<us>(square)) {
        score += kDefendedPawnBonus[rank];
        TRACE_INCREMENT(kDefendedPawnBonus[rank], us);
      }

      // Doubled pawns
      const BitBoard pawns_ahead_on_file =
          our_pawns & masks::forward_file[us][square];
      if (pawns_ahead_on_file) {
        score += kDoubledPawnPenalty[file];
        TRACE_INCREMENT(kDoubledPawnPenalty[file], us);
      }

      // Isolated pawns
      const BitBoard adjacent_pawns = masks::adjacent_files[square] & our_pawns;
      if (!adjacent_pawns) {
        score += kIsolatedPawnPenalty[file];
        TRACE_INCREMENT(kIsolatedPawnPenalty[file], us);
      }
    }

#ifndef TUNE
    cached_pawn_structure_->key = state_.pawn_key;
    cached_pawn_structure_->score[us] = score;
    cached_pawn_structure_->passed_pawns[us] = passed_pawns;
#endif
  } else {
    score = cached_pawn_structure_->score[us];
    passed_pawns = cached_pawn_structure_->passed_pawns[us];
  }

  // Don't cache the king/passed pawn proximity scores as it involves knowing
  // the position of the king, which the pawn cache doesn't store
  const Square king_square = state_.King(us).GetLsb();
  const Square enemy_king_square = state_.King(them).GetLsb();
  for (Square square : passed_pawns) {
    score += kKingPPDistanceTable[square.DistanceTo(king_square)];
    TRACE_INCREMENT(kKingPPDistanceTable[square.DistanceTo(king_square)], us);

    const int dist_to_enemy_king = square.DistanceTo(enemy_king_square);

    score += kEnemyKingPPDistanceTable[dist_to_enemy_king];
    TRACE_INCREMENT(kEnemyKingPPDistanceTable[dist_to_enemy_king], us);

    // Square rule for passed pawns
    const BitBoard enemy_non_pawn_king_pieces =
        state_.KinglessOccupied(them) & ~state_.Pawns(them);
    const int dist_to_promotion = kRank8 - square.RelativeRank<us>();
    if (enemy_non_pawn_king_pieces == 0 &&
        dist_to_promotion < dist_to_enemy_king - (state_.turn == them)) {
      score += kKingCantReachPPBonus;
      TRACE_INCREMENT(kKingCantReachPPBonus, us);
    }
  }

  return score;
}

template <Color us>
ScorePair Evaluation::EvaluateKnights() {
  ScorePair score;

  const BitBoard our_knights = state_.Knights(us);

  for (Square square : our_knights) {
    TRACE_INCREMENT(kPieceValues[kKnight], us);
    TRACE_INCREMENT(kPieceSquareTable[kKnight][square.RelativeTo(us)], us);

    const BitBoard legal_moves =
        LegalizeMoves(kKnight, square, move_gen::KnightMoves(square), us);
    const BitBoard mobility = legal_moves & mobility_zone_[us];

    score += kKnightMobility[mobility.PopCount()];
    TRACE_INCREMENT(kKnightMobility[mobility.PopCount()], us);

    knight_attacks_[us] |= legal_moves;

    const BitBoard enemy_king_attacks = mobility & king_zone_[FlipColor(us)];
    if (enemy_king_attacks) {
      const int king_attack_count = std::min(7, enemy_king_attacks.PopCount());
      attack_power_[us] += kAttackPower[kKnight][king_attack_count];
      TRACE_INCREMENT(kAttackPower[kKnight][king_attack_count], us);
    }

    if (IsOutpostSquare<us>(square)) {
      const int square_offset = 16 * (us == Color::kBlack ? -1 : 1);
      const int relative_square = (square + square_offset).RelativeTo(us);

      score += kKnightOutpostTable[relative_square];
      TRACE_INCREMENT(kKnightOutpostTable[relative_square], us);
    }
  }

  return score;
}

template <Color us>
ScorePair Evaluation::EvaluateBishops() {
  ScorePair score;

  const BitBoard our_bishops = state_.Bishops(us);
  const BitBoard occupied =
      state_.Occupied() ^ state_.Queens(us) ^ state_.Bishops(us);

  if (our_bishops.MoreThanOne()) {
    score += kBishopPairBonus;
    TRACE_INCREMENT(kBishopPairBonus, us);
  }

  for (Square square : our_bishops) {
    TRACE_INCREMENT(kPieceValues[kBishop], us);
    TRACE_INCREMENT(kPieceSquareTable[kBishop][square.RelativeTo(us)], us);

    const BitBoard legal_moves =
        LegalizeMoves(kBishop, square, move_gen::BishopMoves(square, occupied), us);
    const BitBoard mobility = legal_moves & mobility_zone_[us];

    score += kBishopMobility[mobility.PopCount()];
    TRACE_INCREMENT(kBishopMobility[mobility.PopCount()], us);

    bishop_attacks_[us] |= legal_moves;

    const BitBoard enemy_king_attacks = mobility & king_zone_[FlipColor(us)];
    if (enemy_king_attacks) {
      const int king_attack_count = std::min(7, enemy_king_attacks.PopCount());
      attack_power_[us] += kAttackPower[kBishop][king_attack_count];
      TRACE_INCREMENT(kAttackPower[kBishop][king_attack_count], us);
    }

    if (IsOutpostSquare<us>(square)) {
      const int square_offset = 16 * (us == Color::kBlack ? -1 : 1);
      const int relative_square = (square + square_offset).RelativeTo(us);

      score += kBishopOutpostTable[relative_square];
      TRACE_INCREMENT(kBishopOutpostTable[relative_square], us);
    }
  }

  return score;
}

template <Color us>
ScorePair Evaluation::EvaluateRooks() {
  ScorePair score;

  const BitBoard our_rooks = state_.Rooks(us);
  const BitBoard our_pawns = state_.Pawns(us);
  const BitBoard their_pawns = state_.Pawns(FlipColor(us));
  const BitBoard occupied =
      state_.Occupied() ^ state_.Queens(us) ^ state_.Rooks(us);

  for (Square square : our_rooks) {
    TRACE_INCREMENT(kPieceValues[kRook], us);
    TRACE_INCREMENT(kPieceSquareTable[kRook][square.RelativeTo(us)], us);

    const BitBoard legal_moves =
        LegalizeMoves(kRook, square, move_gen::RookMoves(square, occupied), us);
    const BitBoard mobility = legal_moves & mobility_zone_[us];

    score += kRookMobility[mobility.PopCount()];
    TRACE_INCREMENT(kRookMobility[mobility.PopCount()], us);

    rook_attacks_[us] |= legal_moves;

    const BitBoard enemy_king_attacks = mobility & king_zone_[FlipColor(us)];
    if (enemy_king_attacks) {
      const int king_attack_count = std::min(7, enemy_king_attacks.PopCount());
      attack_power_[us] += kAttackPower[kRook][king_attack_count];
      TRACE_INCREMENT(kAttackPower[kRook][king_attack_count], us);
    }

    const BitBoard our_pawns_on_file = our_pawns & masks::files[square];
    if (!our_pawns_on_file) {
      const BitBoard their_pawns_on_file = their_pawns & masks::files[square];
      const bool semi_open_file = their_pawns_on_file != 0;

      score += kRookOnFileBonus[semi_open_file][square.File()];
      TRACE_INCREMENT(kRookOnFileBonus[semi_open_file][square.File()], us);
    }
  }

  return score;
}

template <Color us>
ScorePair Evaluation::EvaluateQueens() {
  ScorePair score;

  const BitBoard our_queens = state_.Queens(us);
  const BitBoard occupied =
      state_.Occupied() ^ state_.Bishops(us) ^ state_.Rooks(us);

  for (Square square : our_queens) {
    TRACE_INCREMENT(kPieceValues[kQueen], us);
    TRACE_INCREMENT(kPieceSquareTable[kQueen][square.RelativeTo(us)], us);

    const BitBoard legal_moves =
        LegalizeMoves(kQueen, square, move_gen::QueenMoves(square, occupied), us);
    const BitBoard mobility = legal_moves & mobility_zone_[us];

    score += kQueenMobility[mobility.PopCount()];
    TRACE_INCREMENT(kQueenMobility[mobility.PopCount()], us);

    queen_attacks_[us] |= legal_moves;

    const BitBoard enemy_king_attacks = mobility & king_zone_[FlipColor(us)];
    if (enemy_king_attacks) {
      const int king_attack_count = std::min(7, enemy_king_attacks.PopCount());
      attack_power_[us] += kAttackPower[kQueen][king_attack_count];
      TRACE_INCREMENT(kAttackPower[kQueen][king_attack_count], us);
    }
  }

  return score;
}

template <Color us>
ScorePair Evaluation::EvaluateKing() {
  ScorePair score;

  const Square square = state_.King(us).GetLsb();
  TRACE_INCREMENT(kPieceSquareTable[kKing][square.RelativeTo(us)], us);

  const Color them = FlipColor(us);

  const BitBoard our_pawns = state_.Pawns(us);
  const BitBoard their_pawns = state_.Pawns(them);

  const int king_rank = square.Rank();
  const int king_file = square.File();

  const BitBoard our_pawns_in_safety_zone = our_pawns & king_zone_[us];
  for (const Square pawn_square : our_pawns_in_safety_zone) {
    const int pawn_rank = pawn_square.Rank();
    const int pawn_file = pawn_square.File();

    constexpr int kKingIndexInZone = 7;
    constexpr int kZoneWidth = 3;

    const int rank_diff = (pawn_rank - king_rank);
    const int file_diff = (pawn_file - king_file);

    const int idx = kKingIndexInZone - (rank_diff * kZoneWidth + file_diff) *
                                           (us == Color::kBlack ? -1 : 1);

    score += kPawnShelterTable[idx];
    TRACE_INCREMENT(kPawnShelterTable[idx], us);
  }

  const Square their_king_square = state_.King(them).GetLsb();
  const int their_king_rank = their_king_square.Rank();
  const int their_king_file = their_king_square.File();

  const BitBoard storming_pawns = our_pawns & pawn_storm_zone_[them];
  for (const Square pawn_square : storming_pawns) {
    const int pawn_rank = pawn_square.Rank();
    const int pawn_file = pawn_square.File();

    constexpr int kKingIndexInZone = 19;
    constexpr int kZoneWidth = 3;

    const int rank_diff = (pawn_rank - their_king_rank);
    const int file_diff = (pawn_file - their_king_file);

    const int idx = kKingIndexInZone - (rank_diff * kZoneWidth + file_diff) *
                                           (them == Color::kBlack ? -1 : 1);

    score += kPawnStormTable[idx];
    TRACE_INCREMENT(kPawnStormTable[idx], us);
  }

  const BitBoard our_pawns_on_file = our_pawns & masks::files[square];
  if (!our_pawns_on_file) {
    const BitBoard their_pawns_on_file = their_pawns & masks::files[square];
    const bool semi_open_file = their_pawns_on_file != 0;

    score += kKingOnFilePenalty[semi_open_file][square.File()];
    TRACE_INCREMENT(kKingOnFilePenalty[semi_open_file][square.File()], us);
  }

  // King danger
  score -= attack_power_[them];

  return score;
}

template <Color us>
ScorePair Evaluation::EvaluateThreats() {
  ScorePair score;

  const Color them = FlipColor(us);
  const BitBoard our_pieces = state_.Occupied(us);
  const BitBoard our_attacks = pawn_attacks_[us] | knight_attacks_[us] |
                               bishop_attacks_[us] | rook_attacks_[us] |
                               queen_attacks_[us];

  for (Square square : knight_attacks_[them] & our_pieces) {
    const auto threatened_piece = state_.GetPieceType(square);
    const bool defended = our_attacks.IsSet(square);
    score += kThreatenedByKnightPenalty[threatened_piece][defended];
    TRACE_INCREMENT(kThreatenedByKnightPenalty[threatened_piece][defended], us);
  }

  for (Square square : bishop_attacks_[them] & our_pieces) {
    const auto threatened_piece = state_.GetPieceType(square);
    const bool defended = our_attacks.IsSet(square);
    score += kThreatenedByBishopPenalty[threatened_piece][defended];
    TRACE_INCREMENT(kThreatenedByBishopPenalty[threatened_piece][defended], us);
  }

  for (Square square : rook_attacks_[them] & our_pieces) {
    const auto threatened_piece = state_.GetPieceType(square);
    const bool defended = our_attacks.IsSet(square);
    score += kThreatenedByRookPenalty[threatened_piece][defended];
    TRACE_INCREMENT(kThreatenedByRookPenalty[threatened_piece][defended], us);
  }

  for (Square square : pawn_attacks_[them] & our_pieces) {
    const auto threatened_piece = state_.GetPieceType(square);
    const bool defended = our_attacks.IsSet(square);
    score += kThreatenedByPawnPenalty[threatened_piece][defended];
    TRACE_INCREMENT(kThreatenedByPawnPenalty[threatened_piece][defended], us);
  }

  // Calculate all squares defended by the enemy, excluding squares that are
  // defended by our pawn(s) and not attacked by their pawn(s)
  const BitBoard enemy_defended_squares =
      pawn_attacks_[them] | ((knight_attacks_[them] | bishop_attacks_[them] |
                              rook_attacks_[them] | queen_attacks_[them]) &
                             ~pawn_attacks_[us]);
  const BitBoard safe_pawn_pushes = move_gen::PawnPushes(state_.Pawns(us), us) &
                                    ~state_.Occupied() &
                                    ~enemy_defended_squares;

  // Calculate which piece would be threatened if we pushed our pawn's here
  const BitBoard pawn_push_threats =
      move_gen::PawnAttacks(safe_pawn_pushes, us) & state_.Occupied(them) &
      ~state_.Pawns(them);
  for (Square square : pawn_push_threats) {
    const auto threatened = state_.GetPieceType(square);
    score += kPawnPushThreat[threatened];
    TRACE_INCREMENT(kPawnPushThreat[threatened], us);
  }

  // Count the number of squares that our pieces can make to place the enemy
  // king in check
  const BitBoard occupied = state_.Occupied();
  const Square their_king_square = state_.King(them).GetLsb();

  const BitBoard rook_checks = move_gen::RookMoves(their_king_square, occupied);
  const BitBoard bishop_checks =
      move_gen::BishopMoves(their_king_square, occupied);

  const BitBoard safe =
      ~(pawn_attacks_[them] | knight_attacks_[them] | bishop_attacks_[them] |
        rook_attacks_[them] | move_gen::KingAttacks(their_king_square));
  const BitBoard safe_knight_checks =
      safe & knight_attacks_[us] & move_gen::KnightMoves(their_king_square);
  const BitBoard safe_bishop_checks =
      safe & bishop_attacks_[us] & bishop_checks;
  const BitBoard safe_rook_checks = safe & rook_attacks_[us] & rook_checks;
  const BitBoard safe_queen_checks =
      safe & queen_attacks_[us] & (bishop_checks | rook_checks);

  score += kSafeCheckBonus[kKnight] * safe_knight_checks.PopCount();
  score += kSafeCheckBonus[kBishop] * safe_bishop_checks.PopCount();
  score += kSafeCheckBonus[kRook] * safe_rook_checks.PopCount();
  score += kSafeCheckBonus[kQueen] * safe_queen_checks.PopCount();

  TRACE_ADD(kSafeCheckBonus[kKnight], safe_knight_checks.PopCount(), us);
  TRACE_ADD(kSafeCheckBonus[kBishop], safe_bishop_checks.PopCount(), us);
  TRACE_ADD(kSafeCheckBonus[kRook], safe_rook_checks.PopCount(), us);
  TRACE_ADD(kSafeCheckBonus[kQueen], safe_queen_checks.PopCount(), us);

  return score;
}

BitBoard Evaluation::LegalizeMoves(PieceType piece,
                                   Square square,
                                   BitBoard moves,
                                   Color us) const {
  if (state_.pinned.IsSet(square)) {
    if (piece == kKnight) {
      moves = 0;
    } else {
      moves &= move_gen::RayIntersecting(square, state_.King(us).GetLsb());
    }
  }

  return moves;
}

Score Evaluation::InterpolateScore(ScorePair score_pair) const {
  const int phase = std::min(state_.phase, kMaxPhase);

  const Score mg_score = score_pair.MiddleGame();
  const Score eg_score = score_pair.EndGame();

  return (mg_score * phase + eg_score * (kMaxPhase - phase)) / kMaxPhase;
}

template <Color us>
[[nodiscard]] bool Evaluation::IsDefendedByPawn(Square square) const {
  return pawn_attacks_[us].IsSet(square);
}

template <Color us>
[[nodiscard]] bool Evaluation::IsOutpostSquare(Square square) const {
  const bool safe_from_pawns =
      IsDefendedByPawn<us>(square) && !IsDefendedByPawn<FlipColor(us)>(square);
  return masks::outposts[us].IsSet(square) && safe_from_pawns;
}

Score Evaluate(const BoardState &state) {
  return Evaluation(state).GetScore();
}

bool StaticExchange(Move move, int threshold, const BoardState &state) {
  const auto from = move.GetFrom();
  const auto to = move.GetTo();

  const PieceType &from_piece = state.GetPieceType(from);
  // Ignore en passant captures and castling
  if ((from_piece == kPawn && to == state.en_passant) ||
      (from_piece == kKing && std::abs(from - to) == 2)) {
    return threshold <= 0;
  }

  // Score represents the maximum number of points the opponent can gain with
  // the next capture
  Score score = kSEEPieceScores[state.GetPieceType(to)] - threshold;
  // If the captured piece is worth less than what we can give up, we lose
  if (score < 0) {
    return false;
  }

  score = kSEEPieceScores[from_piece] - score;
  // If we captured a piece with equal/greater value than our capturing piece,
  // we win
  if (score <= 0) {
    return true;
  }

  const BitBoard &pawns = state.Pawns();
  const BitBoard &knights = state.Knights();
  const BitBoard &bishops = state.Bishops();
  const BitBoard &rooks = state.Rooks();
  const BitBoard &queens = state.Queens();
  const BitBoard &kings = state.Kings();

  BitBoard occupied = state.Occupied();
  occupied.ClearBit(from);
  occupied.ClearBit(to);

  // Get all pieces that attack the capture square
  auto pawn_attackers =
      (move_gen::PawnAttacks(to, Color::kWhite) & state.Pawns(Color::kBlack)) |
      (move_gen::PawnAttacks(to, Color::kBlack) & state.Pawns(Color::kWhite));
  auto knight_attackers = move_gen::KnightMoves(to) & state.Knights();

  BitBoard bishop_attacks = move_gen::BishopMoves(to, occupied);
  BitBoard rook_attacks = move_gen::RookMoves(to, occupied);

  const BitBoard bishop_attackers = bishop_attacks & bishops;
  const BitBoard rook_attackers = rook_attacks & rooks;
  const BitBoard queen_attackers = (bishop_attacks | rook_attacks) & queens;
  const BitBoard king_attackers = move_gen::KingAttacks(to) & kings;

  // Compute all attacking pieces for this square minus the captured and
  // capturing piece
  BitBoard all_attackers = pawn_attackers | knight_attackers |
                           bishop_attackers | rook_attackers | queen_attackers |
                           king_attackers;
  all_attackers &= occupied;

  Color turn = state.turn;
  Color winner = state.turn;

  // Loop through all pieces that attack the capture square
  while (true) {
    turn = FlipColor(turn);
    all_attackers &= occupied;

    const BitBoard our_attackers = all_attackers & state.Occupied(turn);
    // If the current side to move has no attackers left, they lose
    if (!our_attackers) {
      break;
    }

    // Without considering piece values, the winner of an exchange is whoever
    // has more attackers, therefore we set the winner's side to the current
    // side to move only after we check if they can attack
    winner = FlipColor(winner);

    // Find the least valuable attacker
    BitBoard next_attacker;
    int attacker_value;

    if ((next_attacker = our_attackers & pawns)) {
      attacker_value = kSEEPieceScores[kPawn];
      occupied.ClearBit(next_attacker.GetLsb());

      // Add pieces that were diagonal xray attacking the captured piece
      bishop_attacks = move_gen::BishopMoves(to, occupied);
      all_attackers |= bishop_attacks & (bishops | queens);
    } else if ((next_attacker = our_attackers & knights)) {
      attacker_value = kSEEPieceScores[kKnight];
      occupied.ClearBit(next_attacker.GetLsb());
    } else if ((next_attacker = our_attackers & bishops)) {
      attacker_value = kSEEPieceScores[kBishop];
      occupied.ClearBit(next_attacker.GetLsb());

      // Add pieces that were xray attacking the captured piece
      bishop_attacks = move_gen::BishopMoves(to, occupied);
      all_attackers |= bishop_attacks & (bishops | queens);
    } else if ((next_attacker = our_attackers & rooks)) {
      attacker_value = kSEEPieceScores[kRook];
      occupied.ClearBit(next_attacker.GetLsb());

      // Add pieces that were xray attacking the captured piece
      rook_attacks = move_gen::RookMoves(to, occupied);
      all_attackers |= rook_attacks & (rooks | queens);
    } else if ((next_attacker = our_attackers & queens)) {
      attacker_value = kSEEPieceScores[kQueen];
      occupied.ClearBit(next_attacker.GetLsb());

      // Add pieces that were xray attacking the captured piece
      rook_attacks = move_gen::RookMoves(to, occupied);
      bishop_attacks = move_gen::BishopMoves(to, occupied);
      all_attackers |= (rook_attacks & (queens | rooks)) |
                       (bishop_attacks & (queens | bishops));
    } else {
      // King: check if we capture a piece that our opponent is still attacking
      return (all_attackers & state.Occupied(FlipColor(turn)))
               ? state.turn != winner
               : state.turn == winner;
    }

    // Score represents how many points the other side can gain after this
    // capture. If initially a knight captured a queen, the other side can
    // gain 3 - 9 = -6 points. If we flip it and initially a queen captured a
    // knight, the other side can gain 9 - 3 = 6 points
    score = -score + 1 + attacker_value;
    // Quit early if the exchange is lost or neutral
    if (score <= 0) {
      break;
    }
  }

  return state.turn == winner;
}

}  // namespace eval