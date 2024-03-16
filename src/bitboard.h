#ifndef INTEGRAL_BITBOARD_H_
#define INTEGRAL_BITBOARD_H_

#include <iostream>
#include <array>
#include <bit>

#include "types.h"
#include "move.h"

enum PieceBitBoard {
  kWhitePawns,
  kWhiteKnights,
  kWhiteBishops,
  kWhiteRooks,
  kWhiteQueens,
  kWhiteKing,
  kWhitePieces,

  kBlackPawns,
  kBlackKnights,
  kBlackBishops,
  kBlackRooks,
  kBlackQueens,
  kBlackKing,
  kBlackPieces,

  kAllPieces
};

enum Square : int {
  kA1, kB1, kC1, kD1, kE1, kF1, kG1, kH1,
  kA2, kB2, kC2, kD2, kE2, kF2, kG2, kH2,
  kA3, kB3, kC3, kD3, kE3, kF3, kG3, kH3,
  kA4, kB4, kC4, kD4, kE4, kF4, kG4, kH4,
  kA5, kB5, kC5, kD5, kE5, kF5, kG5, kH5,
  kA6, kB6, kC6, kD6, kE6, kF6, kG6, kH6,
  kA7, kB7, kC7, kD7, kE7, kF7, kG7, kH7,
  kA8, kB8, kC8, kD8, kE8, kF8, kG8, kH8,
  kSquareCount = 64,
};

enum RankMask : U64 {
  kRank1 = 0x00000000000000FFULL,
  kRank2 = 0x000000000000FF00ULL,
  kRank3 = 0x0000000000FF0000ULL,
  kRank4 = 0x00000000FF000000ULL,
  kRank5 = 0x000000FF00000000ULL,
  kRank6 = 0x0000FF0000000000ULL,
  kRank7 = 0x00FF000000000000ULL,
  kRank8 = 0xFF00000000000000ULL
};

static const std::array<RankMask, 8> kRankMasks = {
    RankMask::kRank1,
    RankMask::kRank2,
    RankMask::kRank3,
    RankMask::kRank4,
    RankMask::kRank5,
    RankMask::kRank6,
    RankMask::kRank7,
    RankMask::kRank8,
};

enum FileMask : U64 {
  kFileH = 0x8080808080808080ULL,
  kFileG = 0x4040404040404040ULL,
  kFileF = 0x2020202020202020ULL,
  kFileE = 0x1010101010101010ULL,
  kFileD = 0x0808080808080808ULL,
  kFileC = 0x0404040404040404ULL,
  kFileB = 0x0202020202020202ULL,
  kFileA = 0x0101010101010101ULL
};

static const std::array<FileMask, 8> kFileMasks = {
    FileMask::kFileA,
    FileMask::kFileB,
    FileMask::kFileC,
    FileMask::kFileD,
    FileMask::kFileE,
    FileMask::kFileF,
    FileMask::kFileG,
    FileMask::kFileH,
};

class BitBoard {
 public:
  constexpr BitBoard() : bitboard_(0ULL) {}

  BitBoard(U64 bitboard) : bitboard_(bitboard) {}

  static BitBoard from_square(U8 pos) {
    return BitBoard(1ULL << pos);
  }

  [[nodiscard]] inline U64 as_u64() const {
    return bitboard_;
  }

  inline void set_bit(U8 square) {
    bitboard_ |= (1ULL << square);
  }

  inline void clear_bit(U8 square) {
    bitboard_ &= ~(1ULL << square);
  }

  [[nodiscard]] inline bool is_set(U8 square) const {
    return (bitboard_ >> square) & 1;
  }

  inline void move(U8 from, U8 to) {
    bitboard_ ^= (1ULL << from) | (1ULL << to);
  }

  [[nodiscard]] U8 get_lsb_pos() const {
    return std::countr_zero(bitboard_);
  }

  [[nodiscard]] U8 get_msb_pos() const {
    return 63 - std::countl_zero(bitboard_);
  }

  U8 pop_lsb() {
    U8 lsb_pos = get_lsb_pos();
    bitboard_ &= bitboard_ - 1;
    return lsb_pos;
  }

  [[nodiscard]] int pop_count() const {
    return std::popcount(bitboard_);
  }

  BitBoard &operator=(U64 bitboard) {
    bitboard_ = bitboard;
    return *this;
  }

  BitBoard &operator=(BitBoard other) {
    bitboard_ = other.as_u64();
    return *this;
  }

  BitBoard operator&(U64 other) const {
    return BitBoard(bitboard_ & other);
  }

  BitBoard operator&(const BitBoard &other) const {
    return BitBoard(bitboard_ & other.as_u64());
  }

  BitBoard operator|(const BitBoard &other) const {
    return BitBoard(bitboard_ | other.as_u64());
  }

  BitBoard operator|(U64 other) const {
    return BitBoard(bitboard_ | other);
  }

  BitBoard operator|(U64 &other) const {
    return BitBoard(bitboard_ | other);
  }

  BitBoard operator^(U64 other) const {
    return BitBoard(bitboard_ ^ other);
  }

  BitBoard operator^(const BitBoard &other) const {
    return BitBoard(bitboard_ ^ other.as_u64());
  }

  BitBoard operator<<(U8 shift) const {
    return BitBoard(bitboard_ << shift);
  }

  BitBoard operator>>(U8 shift) const {
    return BitBoard(bitboard_ >> shift);
  }

  BitBoard &operator|=(const BitBoard &other) {
    bitboard_ |= other.as_u64();
    return *this;
  }

  BitBoard &operator&=(const BitBoard &other) {
    bitboard_ &= other.as_u64();
    return *this;
  }

  BitBoard &operator&=(U64 other) {
    bitboard_ &= other;
    return *this;
  }

  BitBoard &operator^=(const BitBoard &other) {
    bitboard_ ^= other.as_u64();
    return *this;
  }

  BitBoard &operator*=(const BitBoard &other) {
    bitboard_ *= other.as_u64();
    return *this;
  }

  BitBoard &operator>>=(const BitBoard &other) {
    bitboard_ >>= other.as_u64();
    return *this;
  }

  BitBoard &operator<<=(const BitBoard &other) {
    bitboard_ <<= other.as_u64();
    return *this;
  }

  BitBoard operator~() const {
    return BitBoard(~bitboard_);
  }

  BitBoard operator-(const BitBoard &other) const {
    return BitBoard(bitboard_ - other.bitboard_);
  }

  BitBoard operator-(int num) const {
    return BitBoard(bitboard_ - num);
  }

  BitBoard operator-() const {
    return BitBoard(~bitboard_ + 1);
  }

  BitBoard operator+(const BitBoard &other) const {
    return BitBoard(bitboard_ + other.bitboard_);
  }

  BitBoard operator/(const BitBoard &other) const {
    return BitBoard(bitboard_ / other.bitboard_);
  }

  BitBoard operator*(const BitBoard &other) const {
    return BitBoard(bitboard_ * other.bitboard_);
  }

  bool operator==(const BitBoard &other) const {
    return bitboard_ == other.bitboard_;
  }

  bool operator==(U64 value) const {
    return bitboard_ == value;
  }

  explicit operator bool() const {
    return bitboard_ != 0ULL;
  }

 private:
  U64 bitboard_;
};

using BitBoards = std::array<BitBoard, kAllPieces + 1>;

template<Direction dir>
constexpr inline BitBoard shift(const BitBoard& bitboard) {
  if constexpr (dir == Direction::kNorth)
    return BitBoard(bitboard << 8);
  else if constexpr (dir == Direction::kSouth)
    return BitBoard(bitboard >> 8);
  else if constexpr (dir == Direction::kEast)
    return BitBoard((bitboard << 1) & ~FileMask::kFileA);
  else if constexpr (dir == Direction::kWest)
    return BitBoard((bitboard >> 1) & ~FileMask::kFileH);
  else if constexpr (dir == Direction::kNorthEast)
    return BitBoard((bitboard << 9) & ~FileMask::kFileA);
  else if constexpr (dir == Direction::kNorthWest)
    return BitBoard((bitboard << 7) & ~FileMask::kFileH);
  else if constexpr (dir == Direction::kSouthEast)
    return BitBoard((bitboard >> 7) & ~FileMask::kFileA);
  else if constexpr (dir == Direction::kSouthWest)
    return BitBoard((bitboard >> 9) & ~FileMask::kFileH);
  else
    return BitBoard(0); // default case to avoid compiler warnings, should not be reached
}

inline int rank(U8 square) {
  return square / kBoardRanks;
}

inline int file(U8 square) {
  return square % kBoardFiles;
}

static U8 rank_file_to_pos(int rank, int file) {
  return rank * kBoardLength + file;
}

static void print_bb(BitBoard board) {
  for (int rank = 7; rank >= 0; rank--) {
    for (int file = 0; file < 8; file++) {
      U8 square = rank_file_to_pos(rank, file);
      std::cout << (board.is_set(square) ? '1' : '0');
      if (file < 7)
        std::cout << " ";  // space separator for clarity
    }
    std::cout << std::endl;
  }
}

static std::string get_piece_unicode(BitBoards &pieces, U8 pos) {
  if (pieces[kWhitePieces].is_set(pos)) {
    if (pieces[kWhitePawns].is_set(pos)) return "♙";
    if (pieces[kWhiteKnights].is_set(pos)) return "♘";
    if (pieces[kWhiteBishops].is_set(pos)) return "♗";
    if (pieces[kWhiteRooks].is_set(pos)) return "♖";
    if (pieces[kWhiteQueens].is_set(pos)) return "♕";
    if (pieces[kWhiteKing].is_set(pos)) return "♔";
  } else if (pieces[kBlackPieces].is_set(pos)) {
    if (pieces[kBlackPawns].is_set(pos)) return "♟";
    if (pieces[kBlackKnights].is_set(pos)) return "♞";
    if (pieces[kBlackBishops].is_set(pos)) return "♝";
    if (pieces[kBlackRooks].is_set(pos)) return "♜";
    if (pieces[kBlackQueens].is_set(pos)) return "♛";
    if (pieces[kBlackKing].is_set(pos)) return "♚";
  }
  return " ";
}

static void print_pieces(BitBoards &pieces) {
  for (int rank = kBoardRanks - 1; rank >= 0; rank--) {
    std::cout << rank + 1 << ' ';
    for (int file = 0; file < kBoardFiles; file++) {
      U8 square = rank_file_to_pos(rank, file);
      std::cout << get_piece_unicode(pieces, square);
      if (file < kBoardFiles - 1)
        std::cout << " ";  // space separator for clarity
    }
    std::cout << std::endl;
  }
  std::cout << "  ";
  for (int file = 0; file < kBoardFiles; file++)
    std::cout << static_cast<char>('a' + file) << ' ';
  std::cout << std::endl;
}

#endif // INTEGRAL_BITBOARD_H_