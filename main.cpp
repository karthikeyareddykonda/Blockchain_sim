#include "classes.cpp"
#include <random>
#include <bits/stdc++.h>

// bool operator<(const event& a ,const event& b){


//     return a.time >  b.time;   // gives min priority queue
    
// }

struct hash_pair {
    template <class T1, class T2>
    size_t operator()(const pair<T1, T2>& p) const
    {
        auto hash1 = hash<T1>{}(p.first);
        auto hash2 = hash<T2>{}(p.second);
        return hash1 ^ hash2;
    }
};

// double exponential(double mean,double at_time){
//     default_random_engine generator(rand());
//     exponential_distribution<double> expo(1/mean);
//     double d = at_time + expo(generator);
//     return d;
// }

void make_graph(vector<Node> &network){
    int n = network.size();
    int node = 0;

    queue<int> nodes;
    nodes.push(node);

    default_random_engine generator(rand());
    uniform_real_distribution<double> uniform(10.0,500.0);
    double uni;
    while(!nodes.empty()){
        node = nodes.front();
        nodes.pop();
        int l = node*2+1;
        int r = node*2+2;

        if(l < n){
            network[l].peers.push_back(&network[node]);
            network[node].peers.push_back(&network[l]);
            uni = uniform(generator);
            network[l].latency.push_back(uni);
            network[node].latency.push_back(uni);
            cout << uni << endl;
            nodes.push(l);
        }
        if(r < n){
            network[r].peers.push_back(&network[node]);
            network[node].peers.push_back(&network[r]);
            uni = uniform(generator);
            network[r].latency.push_back(uni);
            network[node].latency.push_back(uni);
            cout << uni << endl;
            nodes.push(r);
        }
    }

    int extra = 0;

    unordered_set<pair<int,int>, hash_pair> edges;


    while(extra--){
        int n1 = rand() % n;
        int n2 = rand() % n;

        if(n1 == n2) continue;
        else if(n1 > n2){
            if(n1 == 2*n2+1) continue;
            else if(n1 == 2*n2+2) continue;
        }
        else{
            if(n2 == 2*n1+1) continue;
            else if(n2 == 2*n1+2) continue;
        }

        if(edges.find({n1,n2}) == edges.end()){
            edges.insert({n1,n2});
            edges.insert({n2,n1});

            network[n1].peers.push_back(&(network[n2]));
            network[n2].peers.push_back(&(network[n1]));
            uni = uniform(generator);
            network[n1].latency.push_back(uni);
            network[n2].latency.push_back(uni);
        }
    }

}



int main(){

    //cout << "okay" << endl;


    int n = 5; 
    float z = 50;

    int num_slow = 10*z/100;
    vector<bool> is_slow(n,false);
    for(int i = 0; i < num_slow; i++){
        is_slow[i] = true;
    }

    random_shuffle(is_slow.begin(), is_slow.end());

    vector<Node> network;
    Block genesis = Block(-1,-1,n);
    priority_queue<event,vector<event>,Compare_event>  pq;
    
    for(int i=0;i<n;i++){
        network.push_back(Node(i, is_slow[i], genesis, n, pq));
    }

    make_graph(network);

    for(int i = 0; i < n; i++){
        cout << "Peers of " << i << " : ";
        for(Node* node: network[i].peers){
            cout << node->id << " ";
        }
        cout << endl;
    }


    srand(time(0));

//-------------- sample for poisson random variable generation -------------

    default_random_engine generator(rand());
    uniform_real_distribution<double> uniform(0.0,1.0);
    

    
    double uni = uniform(generator);
    


















}