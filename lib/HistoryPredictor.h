#ifndef __JJC__HISTORY__PREDICTOR__H__
#define __JJC__HISTORY__PREDICTOR__H__

#include <vector>
#include <initializer_list>
#include "TwoBitCounter.h"
#include "Bits.h"

#define PRINT_BOOL(condi) (condi ? "CORRECT  " : "INCORRECT")

class HistoryPredictor {
private:
    int start_state = 0;
    int number_of_states = 0;
    int state_bits = 0;
    int shift_bit = 1;
    std::vector<TwoBitCounter> tbc_vec;

    void clampState(void) {
        if(start_state < 0)
            start_state = 0;
        else if(start_state >= number_of_states)
            start_state = number_of_states - 1;
    }

public:
    HistoryPredictor(int start_state, int state_bits, std::initializer_list<TBC> l) {
        for(auto s : l) {
            tbc_vec.push_back(TwoBitCounter(s));
        }

        number_of_states = tbc_vec.size();
        this->start_state = start_state;
        this->state_bits  = state_bits;
        this->shift_bit = (1 << (state_bits - 1));
    }

    HistoryPredictor(int start_state, int state_bits, TBC all_pred_state) {
        int num = 1;
        for(int i = 0; i < state_bits; i++)
            num *= 2;

        for(int i = 0; i < num; i++)
            tbc_vec.push_back(TwoBitCounter(all_pred_state));

        number_of_states = tbc_vec.size();

        this->start_state = start_state;
        this->state_bits  = state_bits;
        this->shift_bit   = (1 << (state_bits - 1));
    }

    auto getPrediction(void) -> Branch {
        return tbc_vec[start_state].getPrediction();
    }

    bool update(Branch pred_br, Branch actual_br) {
        
        if(pred_br != actual_br)
            tbc_vec[start_state](actual_br);
        
        start_state >>= 1;
        if(actual_br == Branch::Taken) {
            // shift one bit in if needed
            start_state |= shift_bit;
        }

        this->clampState();

        return (pred_br == actual_br);
    }

    friend std::ostream& operator<<(std::ostream& os, HistoryPredictor& hp) {
        os << '<' << Bits::bitPattern(hp.start_state, hp.state_bits);
        for(auto tbc : hp.tbc_vec) {
            os << ", " << tbc.acr();
        }
        os << '>';

        return os;
    }

    auto runSimulation(std::vector<Branch> pattern, int iters, bool test_dyn = false) -> int {

        std::cout << "Pattern:\n    ";
        for(auto tbc : pattern) {
            std::cout << ((tbc == Branch::Taken) ? "T " : "N ");
        }
        std::cout << "\n\n";

        int hp_print_len = 2 + (4*tbc_vec.size()) + state_bits + 4;
        std::cout << "State";
        for(int i = 0; i < (hp_print_len-5); i++) std::cout << ' ';
        std::cout << "Prediction          Outcome             Correctness\n";

        for(int i = 0; i < hp_print_len; i++)
            std::cout << '-';

        for(int i = 0; i < 60; i++)
            std::cout << "-";
        std::cout << std::endl;

        int correct_in_row = 0;
        long int iterations_run = 0;

        // run the simulation loops
        for(int i = 0; i < iters; i++) {
            auto pred   = getPrediction();
            auto actual = pattern[i % pattern.size()];
            std::cout << *this << "    ";
            std::cout << pred << "    " << actual << "    " << PRINT_BOOL(pred == actual);
            std::cout << "  <-  " << i+1;

            if(pred == actual)
                correct_in_row++;
            else
                correct_in_row = 0;

            if(test_dyn) {
                if(correct_in_row == pattern.size()) {
                    std::cout << "    << DONE\n";
                    return correct_in_row;
                }
            } 
            
            std::cout << std::endl;

            update(pred, actual);
            iterations_run++;
        }

        return correct_in_row;

    }

};

#endif // __JJC__HISTORY__PREDICTOR__H__