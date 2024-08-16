#include<iostream>
//#include<vector>
//#include<string>
//#include<map>
#include<fstream>
#include <iterator>
#include <initializer_list>
#include <utility>
#include<bits/stdc++.h>
using namespace std;

struct Process{
   int id;
   int queue_id ;
   int arrival_time;
   int burst_time;
   int remaining_time;
   int new_arrival;
   int complet_time;
   int final_queue_id;
};

class comp1 {
   public :
   bool operator()(Process n1 , Process n2) const
   {
      if(n1.new_arrival==n2.new_arrival)
      {
        return n1.id < n2.id;
      }
      return n1.new_arrival < n2.new_arrival ;
   }
};

class comp2 {
   public :
   bool operator()(Process n1 , Process n2) const
   {
      if(n1.remaining_time==n2.remaining_time)
      {
        return n1.id < n2.id;
      }
      return n1.remaining_time < n2.remaining_time ;
   }
};

 Process get_process_to_run(map<Process,int,comp1> & queue0 , map<Process,int,comp2>& queue1, map<Process,int,comp2>& queue2, map<Process,int,comp1>& queue3)
 {
    Process p_to_run;
    if(!queue0.empty())
    {
      p_to_run = queue0.begin()->first;
      queue0.erase(queue0.begin());
    }
    else if(!queue1.empty())
    {
      p_to_run = queue1.begin()->first;
      queue1.erase(queue1.begin());
    }
    else if(!queue2.empty())
    {
      p_to_run = queue2.begin()->first;
      queue2.erase(queue2.begin());
    }
    else if(!queue3.empty())
    {
      p_to_run = queue3.begin()->first;
      queue3.erase(queue3.begin());
    }
    else 
    {
       cout<<"Execution complete"<<endl;
    }
    
    return p_to_run;
 }



int main()
{
     int Q,threshold;    // Q = time slice for RR
     string input_file ,  output_file ;
     cin>>Q>>threshold>>input_file>>output_file;
     fstream file;
     file.open(input_file,ios::in);
     
     multimap<int,Process>all_process;  // key= arrival time 
     
     if(!file)
     {
       cout<<"No input file given"<<endl;
       return 0;
     }
     while(1)
     {
     //cout<<file.eof()<<endl;
    
       Process p ;
       //cout<<"arun"<<endl;
       file >> p.id;                 //cout<<p.id<<" ";
        if(file.eof())
     {break;}
       file >> p.queue_id;           //cout<<p.queue_id<<" ";
        if(file.eof())
     {break;}
       file >> p.arrival_time;       //cout<<p.arrival_time<<" ";
        if(file.eof())
     {break;}
       file >> p.burst_time;         //cout<<p.burst_time<<endl;
        if(file.eof())
     {break;}
       p.remaining_time = p.burst_time;
       p.new_arrival= p.arrival_time;
       p.complet_time = -1;
       p.final_queue_id = p.queue_id;
       all_process.insert({p.arrival_time , p });
       
       
     }
     // -- checking correctness of input taken
     //cout<<all_process.size()<<endl;
     
     //for(auto itr = all_process.begin();itr!=all_process.end();++itr)
     //{
       //cout<<itr->first<<" ";
     //}
     
     
     //cout<<endl;
     
      multimap<int,Process>output;
     int total_process = all_process.size(); 
    int instance = all_process.begin()->first;               // Current time 
    //int last_instance = instance;
   
    //vector<map<int,Process>>queue(4);   
    map<Process,int,comp1>queue0;
    map<Process,int,comp2>queue1;
    map<Process,int,comp2>queue2;
    map<Process,int,comp1>queue3;
    //int cnt = 0;
    while(1)
    {
      //cout<<"proc changed"<<endl;
       // -----INSERTING
	    Process P = all_process.begin()->second;                // first arrived Process
	    if( queue0.empty() && queue1.empty() && queue2.empty() && queue3.empty() &&!all_process.empty())
	    {
	       if(P.new_arrival>instance)
	       {
	         instance=P.new_arrival;
	       }
	    }
	    
	    while(P.new_arrival <= instance && !all_process.empty())
	    {
	   // cout<<P.new_arrival<<" t= "<<instance<<endl;
	      all_process.erase(all_process.begin());              // removing taken input from input file stored in map
	      int q = P.queue_id;
	     // cout<<"queue id "<<q<<endl;
	      switch (q)
	      {
		case 4 : queue0.insert({ P , P.new_arrival  }); break;             // Q4 -> Taken for RR        -------(most prior)
		case 3 : queue1.insert({ P , P.remaining_time    }); break;             // Q3 -> Taken for SJF
		case 2 : queue2.insert({ P , P.remaining_time   }); break;             // Q2 -> Taken for SJF
		case 1 : queue3.insert({ P , P.new_arrival  }); break;             // Q1 -> Taken for FCFS       -------(Least prior)
		default: break;
		
	      }
	      
	      P = all_process.begin()->second;
	     // cout<<P.new_arrival<<" t= "<<instance<<endl;
	    }
	    
     //for(auto it :  queue)
    // {
     ////auto it = queue0;
      //for(auto i = it.begin() ; i!= it.end() ; i++)
       //{
     //     cout<<(i->first).id<<" ";
       //}
       //cout<<endl;
     //}
	    
	 // Reshufling due to Threshold 
	 
	 //--------------------------------------------fourth queue
	    
	   //cout<<"arun1"<<endl;
	   if(!queue3.empty())
	    {
	   // cout<<"arun"<<endl;
		 Process p;
		    for(auto it  = queue3.begin() ; it != queue3.end(); it++ )
		    {
		       p = it->first;
		       if(instance-p.new_arrival >= threshold)
		        {
		          //cout<<"Threshold case "<<p.queue_id<<endl;
			       int q = p.final_queue_id;
			       //cout<<"q= "<<q<<endl;
				p.new_arrival = p.new_arrival + threshold ; 
				p.final_queue_id++;      
			       switch (q)
			       {
				case 1 : queue2.insert({ p , p.remaining_time  }); break;     
				case 2 : queue1.insert({ p , p.remaining_time }); break;    
				case 3 : queue0.insert({ p , p.new_arrival  }); break;      
				default: break;
				}
			 it= queue3.erase(it);
			 if(it==queue3.end())
			 {
                           //cout<<"last"<<endl;
                           break;
                        }
			// cout<<queue[i].size()<<endl;
		        }  
		     }
 	     }
 	  
 	  //-------------------------------------------------second queue
 	  
 	  if(!queue2.empty())
	    {
	   // cout<<"arun"<<endl;
		 Process p;
		    for(auto it  = queue2.begin() ; it != queue2.end(); it++ )
		    {
		       p = it->first;
		       if(instance-p.new_arrival >= threshold)
		        {
		          //cout<<"Threshold case "<<p.queue_id<<endl;
			       int q = p.final_queue_id;
			       //cout<<"q= "<<q<<endl;
				p.new_arrival = p.new_arrival + threshold ; 
				p.final_queue_id++;      
			       switch (q)
			       {
				case 1 : queue2.insert({ p , p.remaining_time  }); break;     
				case 2 : queue1.insert({ p , p.remaining_time }); break;    
				case 3 : queue0.insert({ p , p.new_arrival  }); break;      
				default: break;
				}
			 it= queue2.erase(it);
			 if(it==queue2.end())
			 {
                           //cout<<"last"<<endl;
                           break;
                        }
			// cout<<queue[i].size()<<endl;
		        }  
		     }
 	     }
 	     
 	     //-----------------------------------------------------------------------third
 	     
 	     if(!queue1.empty())
	    {
	   // cout<<"arun"<<endl;
		 Process p;
		    for(auto it  = queue1.begin() ; it != queue1.end(); it++ )
		    {
		       p = it->first;
		       if(instance-p.new_arrival >= threshold)
		        {
		          //cout<<"Threshold case "<<p.queue_id<<endl;
			       int q = p.final_queue_id;
			       //cout<<"q= "<<q<<endl;
				p.new_arrival = p.new_arrival + threshold ; 
				p.final_queue_id++;      
			       switch (q)
			       {
				case 1 : queue2.insert({ p , p.remaining_time  }); break;     
				case 2 : queue1.insert({ p , p.remaining_time }); break;    
				case 3 : queue0.insert({ p , p.new_arrival  }); break;      
				default: break;
				}
			 it= queue1.erase(it);
			 if(it==queue1.end())
			 {
                           //cout<<"last"<<endl;
                           break;
                        }
			// cout<<queue[i].size()<<endl;
		        }  
		     }
 	     }
 	     
 	     //-------------------------------------------------------
 	     
 	 
 	    // cout<<"After shufling"<<endl;
	   // for(auto it :  queue)
     //{
     //  for(auto i = it.begin() ; i!= it.end() ; i++)
      // {
      //    cout<<(i->second).id<<" ";
      // }
      // cout<<endl;
    // }
	    
	   // cout<<all_process.size()<<" "<<queue0.size()<<queue1.size()<<queue2.size()<<queue3.size()<<endl;
	// --------------------------------------------------------------------------------Running most prior process
	    Process p_to_run = get_process_to_run(queue0, queue1, queue2, queue3);
	    
	    
	    if(p_to_run.final_queue_id==4 && p_to_run.remaining_time > Q)
	    {
	     // cout<<"running first queue"<<endl;
	    // cout<<"Runnig1 : "<<p_to_run.id<<" "<<instance<<"-"<<instance+Q<<endl;
		 p_to_run.remaining_time -= Q;
		 p_to_run.new_arrival = instance + Q ;
		 queue0.insert({ p_to_run, p_to_run.new_arrival    });
		 instance  += Q; 
		  //auto it = queue0;
		
	     // for(auto i = it.begin() ; i!= it.end() ; i++)
	       //{
		 // cout<<(i->first).id<<" ";
	       //}
	       //cout<<endl;
	      
	    }
	    else
	    {
	   // cout<<"Runnig2 : "<<p_to_run.id<<" "<<instance<<"-"<<instance+p_to_run.remaining_time<<endl;
	  //   cout<<"process complet"<<endl;
	      instance  += p_to_run.remaining_time ; 
	    //  cout<<p_to_run.id<<" "<<p_to_run.queue_id<<" "<<p_to_run.remaining_time<<" "<<instance<<endl;
	      p_to_run.remaining_time = 0;
	      p_to_run.complet_time = instance;
	      
	     // cout<<"Proc having id: "<<p_to_run.id<<" Executed"<<endl;
	      output.insert({p_to_run.id , p_to_run});
	      
	    }
	    //cout<<"After execution"<<endl;
	   // for(auto it :  queue)
    // {
    //   for(auto i = it.begin() ; i!= it.end() ; i++)
     //  {
      //    cout<<(i->second).id<<" ";
     //  }
      // cout<<endl;
    // }
	    //cout<<all_process.size()<<" left-size"<<endl;
	    // cout<<P.new_arrival<<" t= "<<instance<<endl;
	    int size=all_process.size();
	    if(queue0.empty() && queue1.empty() && queue2.empty() && queue3.empty())
	    {
	      if(size==0){break;}
	    }
	   
    
    }
    
    int total_tat = 0;
    int Process_finish_time =0 ;
    ofstream file2;
     file2.open(output_file,ios::in);
    for(auto i = output.begin() ; i!= output.end() ; i++)
       {
       Process p = i->second;
       total_tat+=p.complet_time-p.arrival_time;
       Process_finish_time = max (Process_finish_time , p.complet_time);
       
          file2<<"ID: "<<p.id<<"; Orig. Level: "<<p.queue_id<<"; Final Level: "<<p.final_queue_id<<"; Comp. Time(ms): "<<p.complet_time<<"; TAT (ms): "<<p.complet_time-p.arrival_time<<endl;;
       }
       //cout<<total_tat<<" "<<total_process<<" "<<Process_finish_time<<endl;
       float mean_tat = total_tat/total_process ;
       float Throughput = float(1000*total_process)/Process_finish_time;
       file2<<"Mean Turnaround time: "<<mean_tat<<"(ms): Throughput: "<<Throughput<<" processes/sec"<<endl;
    file2.close();
       return 0;
  
}
