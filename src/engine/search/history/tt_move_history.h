#ifndef INTEGRAL_TT_MOVE_HISTORY_H
#define INTEGRAL_TT_MOVE_HISTORY_H

#include "../../../utils/multi_array.h"
#include "../stack.h"
#include "bonus.h"

namespace search::history {

class TTMoveHistory {
 public:
  TTMoveHistory() : table_({}) {}

  void UpdateTableScore(const BoardState &state, I16 bonus) {
    // Apply a linear dampening to the bonus as the depth increases
    I16 &score = table_[GetIndex(state)][state.turn];
    score += ScaleBonus(score, bonus);
  }

  [[nodiscard]] int GetScore(const BoardState &state) const {
    return table_[GetIndex(state)][state.turn];
  }

 private:
  [[nodiscard]] int GetIndex(const BoardState &state) const {
    return state.pawn_key & 8191;
  }

 private:
  MultiArray<I16, 8192, kNumColors> table_;
};

}  // namespace search::history

#endif  // INTEGRAL_TT_MOVE_HISTORY_H
