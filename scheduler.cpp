// this is the only file you should modify and submit for grading

#include "scheduler.h"
#include "common.h"
#include "iostream"

void simulate_rr(
    int64_t quantum, 
    int64_t max_seq_len,
    std::vector<Process> & processes,
    std::vector<int> & seq
) {
    seq.clear();
    int64_t curr_time =0;
    //int64_t remaining_slice = quantum;
    int cpu = -1;
    std::vector<int> rq, jq;
    std::vector<int64_t> remaining_bursts;
    for(auto & process : processes){
        jq.push_back(process.id);
        remaining_bursts.push_back(process.burst);
    }

    while(1){
        cpu = -1;
        if(jq.empty() && (int)rq.size()==1){
            curr_time+=remaining_bursts[rq[0]];
            cpu = rq[0];
            remaining_bursts[rq[0]]=0;
            if(seq.back()!=cpu && (int)seq.size()<max_seq_len)  seq.push_back(cpu);
            processes[rq[0]].finish_time = curr_time;
            break;
        }
        if(rq.empty() && jq.empty()) break;

        if(!jq.empty() && !rq.empty()){
            while(processes[jq[0]].arrival_time<curr_time && !jq.empty()){
                rq.push_back(jq[0]);
                jq.erase(jq.begin());             
            }
            if(!jq.empty() && processes[jq[0]].arrival_time == curr_time){
                if(remaining_bursts[rq[0]]!=0){
                    int current_process = rq[0]; 
                    rq.erase(rq.begin());
                    rq.push_back(current_process);
                }else  rq.erase(rq.begin());   
                rq.push_back(jq[0]);
                jq.erase(jq.begin());
            }else{
                
                if(remaining_bursts[rq[0]]!=0){
                    int current_process = rq[0]; 
                    rq.erase(rq.begin());
                    rq.push_back(current_process);
                }else rq.erase(rq.begin());   
            }   
        }
        if(rq.empty() && !jq.empty()){
            rq.push_back(jq.at(0));
            jq.erase(jq.begin());
            if(curr_time == processes[rq[0]].arrival_time) cpu = rq[0];
            curr_time = processes.at(rq[0]).arrival_time;
            if((int)seq.size()<max_seq_len)  seq.push_back(cpu);
        }
        int crazy_optimization_is_possible = 1;
        int64_t n = 1;
        if(!rq.empty() && !jq.empty() && curr_time+(int)rq.size()*quantum <processes[jq[0]].arrival_time){
            int64_t minimum_remaining_burst = remaining_bursts[rq[0]];
            for(int i=0;i<(int)rq.size();i++){
                if(remaining_bursts[rq[i]] <= quantum) crazy_optimization_is_possible=0;
                if((remaining_bursts[rq[i]])<minimum_remaining_burst) minimum_remaining_burst = remaining_bursts[rq[i]];
            }
            int64_t maybe_n1 = minimum_remaining_burst/quantum;
            if(minimum_remaining_burst%quantum==0) maybe_n1--;
            int64_t maybe_n2 = (processes[jq[0]].arrival_time-curr_time)/((int)rq.size()*quantum);
            n = std::min(maybe_n1,maybe_n2 );
        }else if(!rq.empty() && jq.empty()){
            int64_t minimum_remaining_burst = remaining_bursts[rq[0]];
            for(int i=0;i<(int)rq.size();i++){
                if(remaining_bursts[rq[i]] < quantum) crazy_optimization_is_possible=0;
                if(remaining_bursts[rq[i]] == quantum) crazy_optimization_is_possible=0;
                if((remaining_bursts[rq[i]])<minimum_remaining_burst) minimum_remaining_burst = remaining_bursts[rq[i]];
            }
            n = minimum_remaining_burst/quantum;
            if(minimum_remaining_burst%quantum==0) n--;  
        }else crazy_optimization_is_possible=0;

        if(!rq.empty() && crazy_optimization_is_possible){          
            for(int i=0;i<(int)rq.size();i++){
                if(processes[rq[i]].start_time == -1) processes[rq[i]].start_time=curr_time+i*quantum;         
                remaining_bursts[rq[i]]-=n*quantum; 
            }
            for(int64_t i=0;i<n;i++){ 
                if(i >=(int64_t) max_seq_len) break;  
                for(int j=0;j<(int)rq.size();j++){ 
                    cpu = rq[j];
                    if(seq.back()!=cpu && (int)seq.size()<max_seq_len)  seq.push_back(cpu);
                }    
            }              
            curr_time+=n*rq.size()*quantum;    
        }
        
        if(!rq.empty() && remaining_bursts[rq[0]]<=quantum ){
          
            if(processes[rq[0]].start_time == -1) processes[rq[0]].start_time=curr_time;
            cpu = rq[0];
            curr_time+=remaining_bursts[rq[0]];
            if(seq.back()!=cpu && (int)seq.size()<max_seq_len)  seq.push_back(cpu);   
            remaining_bursts[rq[0]] = 0;
            processes[rq[0]].finish_time = curr_time;
        }
        else if(!rq.empty() && remaining_bursts[rq[0]] > quantum){
            if(processes[rq[0]].start_time == -1) processes[rq[0]].start_time=curr_time;
            cpu = rq[0];
            curr_time+=quantum;
            if(seq.back()!=cpu && (int)seq.size()<max_seq_len)  seq.push_back(cpu);
            remaining_bursts[rq[0]] -= quantum;
        }

        if(!rq.empty() && jq.empty()){
            if(remaining_bursts[rq[0]]!=0){
                int current_process = rq[0]; 
                rq.erase(rq.begin());
                rq.push_back(current_process);
            } else  rq.erase(rq.begin());

        }       
    } 
    return;
}
