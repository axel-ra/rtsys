/*
Name: Axel Rolando Alvarenga Munoz
UH ID: 2075248
COSC 4331
Bonus project
*/

#include <iostream>
#include <vector>
#include <map>
#include <queue>
#include <cmath> // https://en.cppreference.com/w/cpp/numeric/math/ceil
#include <algorithm>
#include <fstream> // https://www.w3schools.com/cpp/cpp_files.asp

using namespace std;

struct task{
    int tnum = -1; // task number (unique to each task)
    int a = -1; // arrival time
    int c = -1;
    int d = -1;
    int s = -1; // switch time

    int remaining_c = -1;

    task(int _tnum, int _a, int _c, int _d, int _s){
        this->tnum = _tnum;
        this->a = _a;
        this->c = _c;
        this->d = _d; 
        this->s = _s; 
        this->remaining_c = _c;
    }

    // https://stackoverflow.com/questions/15601973/stl-priority-queue-of-c-with-struct
    bool operator<(const task& rhs) const{
        return d > rhs.d;
    }
};

int t = 0; // time

map<int,vector<task>> timeMap;
priority_queue<task> currentScheduleQueue; // https://www.geeksforgeeks.org/stl-priority-queue-for-structure-or-class/#
queue<task*> taskSet;
priority_queue<task> accepted_task_set;
double Ut, mainUt;
bool updateCPUspeed = true;
vector<string> schedule_strings;
vector<double> Ut_choices = {};

void schedule(){
    int t_copy = t;

    while (accepted_task_set.size() > 0) {
    
        if (updateCPUspeed == false){
            if (currentScheduleQueue.top().remaining_c == 0){
                currentScheduleQueue.pop();
                break;
            }
            else {
                cout << "Error. This line is not suppossed to be printed." << endl;
            }
        }
        else {
            vector<double> Ujt_vector;

            for (int m = 1; m <= accepted_task_set.size(); m++){
                vector<task> tasks_popped;
                int j = m;
                int cur_Ujt_numerator_sum = 0;

                for (int i = 1; i <= j; i++){
                    task tmp = accepted_task_set.top();
                    cur_Ujt_numerator_sum += tmp.remaining_c;

                    tasks_popped.push_back(tmp);
                    accepted_task_set.pop();
                }

                // restoring the priority queue
                task tmp2 = tasks_popped.back();
                while (tasks_popped.size() > 0){
                    accepted_task_set.push(tasks_popped.back());
                    tasks_popped.pop_back();
                }

                double Ujt = (cur_Ujt_numerator_sum)/(tmp2.d*1.00 - t*1.00); 
                Ujt_vector.push_back(Ujt);
            }

            Ut = *max_element(Ujt_vector.begin(), Ujt_vector.end()); // https://www.geeksforgeeks.org/how-to-find-the-maximum-element-of-a-vector-using-stl-in-c/#

            int k; // kth task (explore all the tasks)
            for (int i = 0; i < Ujt_vector.size(); i++){
                if (Ut == Ujt_vector.at(i)){
                    // to avoid breaking any possible ties, 
                    // just overwrite Ut (in the end, Ut will
                    // have the value of the greatest i that matched
                    // the value of Ut)
                    Ut = Ujt_vector.at(i);
                    k = i;
                }
            }

            for (int i = 0; i < Ut_choices.size(); i++){
                if (Ut_choices.at(i) > Ut){
                    Ut = Ut_choices.at(i);
                    break;
                }
            }

            mainUt = Ut; // mainUt used in CPU function

            // line 4 of the scheduling algorithm:
            // Schedule only tasks in [t, Dk] at the voltage
            // corresponding to processor speed U(t) and remove
            // the tasks from the set S

            // "Schedule" means, add to the priority queue that the
            // CPU reads for execution
            int Dk = -1;
            priority_queue<task> accepted_task_set_copy = accepted_task_set; // only to find locations without affecting the queue
            for (int i=0; i < accepted_task_set.size(); i++){
                if (i == k){
                    Dk = accepted_task_set_copy.top().d;
                    accepted_task_set_copy.pop();
                }
                else {
                    accepted_task_set_copy.pop();
                }
            }

            // empty the current schedule to make room for the new one
            while (currentScheduleQueue.size() > 0){
                currentScheduleQueue.pop();
            }

            int accepted_task_set_size_int = accepted_task_set.size();
            for (int i = 0; i < accepted_task_set_size_int; i++){
                if ((accepted_task_set.top().d <= Dk)){

                    // modify the computation time to reflect
                    // the change in the CPU speed
                    task tmp_task = accepted_task_set.top();
                    double increase = 1 + (Ut);

                    // take the ceiling of it
                    tmp_task.remaining_c = ceil((tmp_task.remaining_c + tmp_task.s*2)*(1.00 + (1.00 - Ut)));
                    
                    // push the task with the modified remaning computation time
                    currentScheduleQueue.push(tmp_task);
                    accepted_task_set.pop();
                }
            }

            t_copy = Dk;

        }
    }
}



void accept(task new_task){
    vector<double> Ujt_vector;
    priority_queue<task>  accepted_task_set_copy = accepted_task_set;

    accepted_task_set = currentScheduleQueue;
    accepted_task_set.push(new_task);

    for (int m = 1; m <= accepted_task_set.size(); m++){
        vector<task> tasks_popped;
        int j = m;
        int cur_Ujt_numerator_sum = 0;

        for (int i = 1; i <= j; i++){
            task tmp = accepted_task_set.top();
            cur_Ujt_numerator_sum += tmp.remaining_c;

            tasks_popped.push_back(tmp);
            accepted_task_set.pop();
        }

        // restoring the priority queue
        task tmp2 = tasks_popped.back();
        while (tasks_popped.size() > 0){
            accepted_task_set.push(tasks_popped.back());
            tasks_popped.pop_back();
        }

        double Ujt = (cur_Ujt_numerator_sum)/(tmp2.d*1.00 - t*1.00); 
        Ujt_vector.push_back(Ujt);
    }

    Ut = *max_element(Ujt_vector.begin(), Ujt_vector.end()); // https://www.geeksforgeeks.org/how-to-find-the-maximum-element-of-a-vector-using-stl-in-c/#

    for (int i = 0; i < Ut_choices.size(); i++){
        if (Ut_choices.at(i) > Ut){
            Ut = Ut_choices.at(i);
            break;
        }
    }

    if (Ut > 1){
        //cout << "Current accepted task set cannot be scheduled" << endl;
        //cout << "even at the highest speed using EDF scheduling algorithm" << endl;
        cout << "New task rejected to the system|" << endl;
        
        // since new_task cannot be accepted,
        // remove the task requesting service
        // idea: pop pop until you find new_task,
        // pop new_task without pushing it back,
        // push the tasks back into the priority queue

        vector<task> tmp_vector;
        for (int i = 0; i < accepted_task_set.size(); i++){
            if (accepted_task_set.top().tnum == new_task.tnum){
                accepted_task_set.pop(); // pop and do not push back
            }
            else {
                // pop
                tmp_vector.push_back(accepted_task_set.top());
                accepted_task_set.pop();
            }
        }

        // push the elements back to restore the queue
        for (int i = 0; i < tmp_vector.size(); i++)
            accepted_task_set.push(tmp_vector.at(i));


    }
    else if ((Ut < 1) && (Ut > 0)) {
        cout << "Acceptance test passed, new task accepted, and Ut is " << Ut << endl; 
        cout << "Now calling scheduler function." << endl;
        schedule();
    }
    else {
        cout << "Something went wrong, Ut value is " << Ut << endl;  
    }

}

void CPU(){
    task current_task = task(-1, -1, -1, -1, -1);
    int idle_max = 5;
    while (t < 200){
        
        bool task_is_arriving = (timeMap[t].size() > 0);
        
        if (task_is_arriving){
            for (int i = 0; i < timeMap[t].size(); i++){
                if (current_task.tnum != -1){
                    cout << "task " << timeMap[t].at(i).tnum << " arrives" << ", task " << current_task.tnum << " preempted|" << endl;
                    schedule_strings.push_back("time:" + to_string(t) + "; t" + to_string(timeMap[t].at(i).tnum) + " arrives" +
                                                ", task " + to_string(current_task.tnum) + " preeempted");  
                }
                // push the task for acceptance test
                accepted_task_set.push(timeMap[t].at(i));
                accept(timeMap[t].at(i));
            }
        }

        if (currentScheduleQueue.size() > 0){

            task prev_task = current_task;

            if ((prev_task.s != -1) && 
                (prev_task.tnum != currentScheduleQueue.top().tnum)){

                for (int i = 0; i < 1; i++){
                    cout << "S: from previous task" << prev_task.tnum << " for " << prev_task.s << " units of time.|" << endl;
                    schedule_strings.push_back("time:" + to_string(t) + "; Switch from previous task t" + to_string(prev_task.tnum) +
                                                " for " + to_string(prev_task.s) + " units of time.");
                    t = t + prev_task.s;
                }

                for (int i = 0; i < 1; i++){
                    cout << "S: to current task t" << currentScheduleQueue.top().tnum << " for " << currentScheduleQueue.top().s << " units of time.|" << endl;
                    schedule_strings.push_back("time:" + to_string(t) + "; Switch to current task t" + to_string(currentScheduleQueue.top().tnum) +
                                                " for " + to_string(currentScheduleQueue.top().s) + " units of time.");
                    t = t + currentScheduleQueue.top().s;
                }
            }

            while (1){
                if (prev_task.tnum != currentScheduleQueue.top().tnum){
                    cout << "t" << currentScheduleQueue.top().tnum << " running for 1 unit of time|" << endl;
                    schedule_strings.push_back("time:" + to_string(t) + "; t" + to_string(currentScheduleQueue.top().tnum) + " runs" + " Ut is " + to_string(mainUt));    
                }
                else {
                    cout << "t" << currentScheduleQueue.top().tnum << " running for 1 unit of time|" << endl;
                    schedule_strings.push_back("time:" + to_string(t) + "; t" + to_string(currentScheduleQueue.top().tnum) + " runs" + " Ut is " + to_string(mainUt)); 
                }
                t++;
                task tmp = currentScheduleQueue.top();
                currentScheduleQueue.pop();
                tmp.remaining_c--;
                currentScheduleQueue.push(tmp);
                prev_task = currentScheduleQueue.top(); // make myself the previous task
                current_task = currentScheduleQueue.top(); // make myself the current task
                break;
            }

            // if the task has finished its execution, create
            // a new schedule
            if (currentScheduleQueue.top().remaining_c == 0){
                updateCPUspeed = false;

                // since I am not passing the queue to schedule function I am modifying it here
                accepted_task_set = currentScheduleQueue;
                schedule(); // call schedule function to create the new schedule
            }

        }
        else {
            t++;
        }
    }
}


int main(){

    cout << "Starting the program" << endl;
    
    /****************************************/
    /***************************************/
    /* add the path to the input file below */

    string path_to_file = "input2.txt";

    /****************************************/
    /****************************************/


    ifstream input(path_to_file);
    string tmp_str;
    vector<string> input_lines;

    // credit for strategy/code to read file: https://www.w3schools.com/cpp/cpp_files.asp
    while (getline(input, tmp_str)){
        input_lines.push_back(tmp_str);
    }
    input.close();
    //

    // Note: For parsing the input, I am ignoring the first and the second lines
    // (I am not using the number of tasks (1st line) and the 2nd line is just 
    // and empty line)

    // parsing the input from line 3 (where the data for the possible CPU 
    // speeds is)
    int size_of_line_three = input_lines.at(2).size();
    string CPUspeeds_string = input_lines.at(2);
    int z = 0;
    while (z < size_of_line_three){
        if (CPUspeeds_string.at(z) == '['){
            // get the numbers (convert them to doubles)
            string decimals;
            z++;
            while ( z < size_of_line_three){
                string allowed_speed_str;
                double allowed_speed;

                while (CPUspeeds_string.at(z)!=',' && CPUspeeds_string.at(z)!=')'){
                    allowed_speed_str += CPUspeeds_string.at(z);
                    z++;
                }
                z = z + 2; // to skip the space character
                allowed_speed = stod(allowed_speed_str);
                // once you get a number push it to the corresponding vector

                Ut_choices.push_back(allowed_speed);
            }
        }
        z++;
    }


    
    // parsing the input from line 4 onwards (where the data for the tasks is)
    for (int i = 3; i < input_lines.size(); i++){
        string a_string, c_string, d_string, s_string;
        int a_int, c_int, d_int, s_int;

        string task_string = input_lines.at(i);
        int line_size = task_string.size();
        int k = 0;
        while (k < line_size){
            if (task_string.at(k) == '['){

                //get the values for the task
                
                k++;
                while (isdigit(task_string.at(k))){
                    a_string += task_string.at(k);
                    k++;
                }
                a_int = stoi(a_string);

                k++;
                while (isdigit(task_string.at(k))){
                    c_string += task_string.at(k);
                    k++;
                }
                c_int = stoi(c_string);

                k++;
                while (isdigit(task_string.at(k))){
                    d_string += task_string.at(k);
                    k++;
                }
                d_int = stoi(d_string);

                k++;
                while (isdigit(task_string.at(k))){
                    s_string += task_string.at(k);
                    k++;
                }
                s_int = stoi(s_string);

                // when the closing ']' is found, break from the loop
                if (task_string.at(k) == ']'){
                    break;
                }

            }
            k++;
        }

        task* tmp_task = new task(i, a_int, c_int, d_int, s_int);
        taskSet.push(tmp_task);
    }

    queue<task*> tmp = taskSet;

    // initializing the map that maps the time units
    // to the tasks that arrive at that time unit
    for (int i = 0; i < 200; i++){
        vector<task> tmp_vector;
        timeMap[i] = tmp_vector;
    }

    // uploading the necessary data to the map
    while (tmp.size() > 0){
        for (int i = 0; i < timeMap.size(); i++){
            if (i == tmp.front()->a){
                timeMap[i].push_back(*tmp.front()); // put the actual value in timeMap (tmp.front() is a pointer)
                tmp.pop();
                break;
            }
        }
    }

    CPU(); // simulates the execution and preemption of tasks, and calls the acceptance function

    // printing the schedule
    cout << "End of program" << endl;
    cout << "The speed choices are: " << endl;
    for (int i = 0; i < Ut_choices.size(); i++){
        cout << Ut_choices.at(i) << endl;
    }
    cout << "----Printing the schedule (Below)----" << endl;
    for (int i = 0; i < schedule_strings.size(); i++){
        cout << schedule_strings.at(i) << endl;
    }
    cout << "---------End-----------------" << endl;

    return 0;
}