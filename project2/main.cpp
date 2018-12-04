#include <iostream>
#include <string>

#include <reservation_station.h>
#include <register_file.h>
#include <instruction_queue.h>
#include <file_parser.h>
#include <functional_unit.h>
#include <main.h>

using namespace std;

void print_reservation_stations(reservation_station_t* rs);
auto int_to_string_padded(int i) -> std::string;
void print_all_hardware(
        reservation_station_t* rs, 
        register_file_t& rf, 
        functional_unit_t* fu, 
        instruction_queue_t& iq);

int main(int argc, char* argv[]) {
    register_file_t RF;
    reservation_station_t stations[5];
    instruction_queue_t iq;
    functional_unit_t fu[2]; // fu[0] +- fu[1] */
    file_parser_t fp(argv[1], iq, RF);

    for(;;) {
        simulate_cycle(stations, iq, RF, fu);
        print_all_hardware(stations, RF, fu, iq);
        getchar();
    }

    return 0;
}

void print_all_hardware(
        reservation_station_t* rs, 
        register_file_t& rf, 
        functional_unit_t* fu,
        instruction_queue_t& iq) {
    cout << "#=============================================\n\n";
    print_reservation_stations(rs);
    cout << rf << endl;
    cout << fu[0] << fu[1] << endl;
    cout << iq << endl;
}

auto int_to_string_padded(int i, int len) -> std::string {
    auto str = to_string(i);
    while(str.size() < len)
        str.push_back(' ');
    return str;
}

void print_reservation_stations(reservation_station_t* rs) {
    cout << "+----+-------+-----------+------------+------------+------+------+\n";
    cout << "| v  |  busy | operation |     Vj     |     Vk     |  Qj  |  Qk  |\n";
    cout << "+----+-------+-----------+------------+------------+------+------+\n";

    for(int i : {0, 1, 2, 3, 4}) {
        cout << "| " << i << "  |";
        if(!rs[i].busy) {
            cout << " false |";
        } else {
            cout << " true  |";
        }

        switch(rs[i].current_operation) {
            case operation_t::_add:
                cout << "    add    |"; break;
            case operation_t::_subtract:
                cout << "  subtract |"; break;
            case operation_t::_multiply:
                cout << "  multiply |"; break;
            case operation_t::_divide:
                cout << "   divide  |"; break;
            case operation_t::_none:
                cout << "           |"; break;
            default:
                cout << "  UNKNOWN  |"; break;
        }
        
        cout << ' ' << int_to_string_padded(rs[i].Vj, 11) << '|';
        cout << ' ' << int_to_string_padded(rs[i].Vk, 11) << '|';

        if(rs[i].Qj == -1) {
            cout << "  ### |";
        } else {
            cout << " RS" << int_to_string_padded(rs[i].Qj, 3) << '|';
        }

        if(rs[i].Qk == -1) {
            cout << "  ### |";
        } else {
            cout << " RS" << int_to_string_padded(rs[i].Qk, 3) << '|';
        }

        cout << endl;
    }

    cout << endl;

}
