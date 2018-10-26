
    A completely modular and configurable 
    Tomasulos Algorithm simulator

    Every instance must have:
        ASIDE.) Since the inclusion of the ConfigGenerator class, 
            you can simply make a separatecpu config file and let it 
            generate all of the simulation components needed. 
            otherwise follow the steps below:

        a.) a register file with well-defined number of registers.
            this is something that you must know ahead of time, 
            though I could write a configuration generator

        b.) a single instruction queue. the queue is realized as 
            a std::vector and the InstructionEntry class takes 
            care of tracking all information needed to properly 
            execute a single instruction

            currently, the instruction queue is responsible for 
            parsing the input file and creating the proper 
            instruction sequences. i would like to separate 
            this functionality

        c.) One or more reservation station groups. a group of 
            stations is associated with an execution unit that 
            is managed internally (dont worry about it)

            when a ReservationStationEntry is instantiated, a 
            reference to it is stored in an internal queue that 
            maintains its order across multiple method calls and 
            is accessed like a global pointer table. each entry, 
            therefore, only needs to maintain in index into this table

        d.) an instance of TomasuloUnit class. this takes care of 
            passing data between all of the components given. it is 
            written in a very general-purpose way so you can have 
            any hardware configuration you'd like

    NOTES:
        1.) This simulator does not currently support 
            a re-order buffer

        2.) getchar() is used to pause the simulator 
            between simulated clock cycles

        3.) All modular parts of this system have been 
            defined with an operator<<(std::ostream&, OBJECT&) 
            to allow easy printing. operator overloading ftw

        4.) all relevant system files are in the tomasulo 
            directory which has the same parent directory 
            as this one

        5.) I would have liked to use Ncurses to make the 
            printing of various subsystems better aligned. 
            i didnt/dont know if the lab computers have the 
            correct libraries installed

        6.) compile parameters:
            OS:        Ubuntu 16.04
            IDE:       Visual Studio Code
            Compiler:  g++ (v5.4.0)
            Flags:     -std=c++11 -O3 -march=native -I./../tomasulo
            Language:  C++ (obviously)
            Note:      
                This program compiled on my computer with 0 warnings. It 
                also extensively uses features found only in the C++11 
                standard and newer (auto, lambdas, etc.). It WILL NOT 
                COMPILE with a non-c++11 standards compliant compiler

        7.) Normally, for project of this scale, I would split my library 
            files into .h and .cpp, then compile them into .o, then simply 
            recompile specific files when needed (many tools exist to 
            do just this). however, to simplify the compilation 
            process, i simply put all of my code into header files. I did 
            it this way because I am unfamiliar with the standard ways of 
            compiling software for Windows. This does, of course, mean that 
            the entire thing must be recompiled every time a small change 
            is made to any file

            I was able to compile this program with a single line of shell 
            script which can be found in the build.sh file

        8.) Licensing:
            This software is released under the terms of the 'unlicense'. I 
            have relinquished, in perpetuity, all copyright claims to this 
            software. One directory above this one is a LICENSE file with 
            more information including the full terms of the Unlicense
