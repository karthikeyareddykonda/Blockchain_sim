#include<bits/stdc++.h>
#include <random>
#include <time.h>
using namespace std;

#define MAX_BLOCK_SIZE 3
#define MAX_BLOCKS_GEN 1
#define BLOCK_INTERARRIVAL 1000
//srand(time(0));
//default_random_engine generator2(rand());

double exponential(double mean,double at_time){
    default_random_engine generator(rand());
    exponential_distribution<double> expo(1/mean);
    double d = at_time + expo(generator);
    return d;
}

class Txn{

public:

    string s;
    int txn_id;
    int sender_id;
    int receiver_id;
    double amount;
    bool coinbase;

    Txn (int id, int sid,int rid,double trans_amount, bool coinbase = false){

        // default_random_engine generator2(rand());
        // uniform_int_distribution<int> uni(0,1000000);
        // txn_id = uni(generator2);
        txn_id = id;
        cout << "Transaction created id: " << txn_id << endl;

        sender_id = sid;
        receiver_id = rid;
        amount = trans_amount;
        this->coinbase = coinbase;
       
        //generating appropriate string and store in s (separate case for coinbase)

        if(coinbase){
            s += std::to_string(txn_id);
            s += ":";
            s += std::to_string(receiver_id);
            s += " mines 50 coins";
        }
        else{
            s += std::to_string(txn_id);
            s += ": ";
            s += std::to_string(sender_id);
            s += " pays ";
            s += std::to_string(receiver_id);
            s += " ";
            s += std::to_string(amount);
            s += " coins";
        }

    }

    Txn (string str){
        s = str;

        // parse the string etc
        if(str.find("mines") != string::npos){
            // coinbase transaction
            
            sender_id = -1;
            amount = 50;
            coinbase = true;

            int ind = str.find(':');
            txn_id = stoi(str.substr(0,ind));

            receiver_id = stoi(str.substr(ind+1,-1));
        }
        else{
            // normal transaction

            int ind = str.find(':');
            txn_id = stoi(str.substr(0,ind));

            ind = str.find(' ');
            sender_id = stoi(str.substr(ind+1,-1));
            
            ind = str.find(' ', ind+1);
            ind = str.find(' ', ind+1);
            receiver_id = stoi(str.substr(ind+1,-1));

            ind = str.find(' ', ind+1);
            amount = stoi(str.substr(ind+1,-1));
            
            coinbase =  false;

        }

    }

    //default constructor (dummy functionality to prevent error in event class)

    Txn () {
        sender_id = -1;
    }

    string to_string(){
        return s;
    }




};

class Block{

public:

    int id;
    int parent_id;

    unordered_set<string> transactions; 



    // normal block constructor
    Block(int parent_id, int miner_id, int num_nodes){
        int new_id = rand()%INT_MAX+1;
        cout << "block created id : " << new_id << endl;
        
        id = new_id;
        this->parent_id = parent_id;

        // Adding coinbase transaction
        srand(new_id);
        int txn_id = rand()%INT_MAX+1;
        Txn coinbase = Txn(txn_id, -1, miner_id, 50, true);
        transactions.insert(coinbase.to_string());


    }

    // default constructor

    Block(){
        parent_id = -1;
        id = 0;
    }





};


class Block_node{

public:

    int id;
    int length;         // distance from the genesis block_node

    unordered_set<string> transactions; 
    vector<int> balances;
    vector<Block_node*> children;
    Block_node* parent;

    //TODO
    vector<int> counts;  //  to track the family tree counts , should use dfs/bfs

    // constructor
    Block_node(int id, int length, unordered_set<string> transactions, vector<int> balances, Block_node* parent = NULL){
        this->id = id;
        this->length = length;
        this->transactions = transactions;
        this->balances = balances;
        this->parent = parent;

    }

    // adding of child (returns bool to indicate success)
    Block_node* add_child(Block* new_block, vector<int> temp_balances){

        // Verification successful, making a new Block_node to add in the tree
        Block_node* new_block_node = new Block_node(new_block->id, this->length+1, new_block->transactions, temp_balances, this);
        children.push_back(new_block_node);

        return new_block_node;
    }

    Block_node* find(int block_id){
        if(id == block_id){
            return this;
        }
        else{
            Block_node* result = NULL;
            for(Block_node* block: children){
                result = block->find(block_id);

                if(result != NULL) return result;
            }

            return result;
        }
    }

};

class event{


    public :
    int type; // type of event, block recieve or txn recieve 
    double time; // the time at which event to be happened
    int id; // id of transaction or block
    int sender_id;
    int receiver_id;
    Txn trans;
    Block B;
    // possible extra information regarding event, sender reciever, txn, block
    // here sender is the network sender , not the one in txn
    

    // constructor

    // the forwarding txn constructor, node should trigger check and broadcast
    event (Txn tn, int sid, int rid , double at_time){
        trans = tn;
        time = at_time;
        type = 3;
        sender_id = sid;
        receiver_id = rid;
    }

    // forwarding or receive block constructor , node should trigger check and broadcast
    event (Block cur_block,int sid,int rid, double at_time){
        B = cur_block;
        time = at_time;
        sender_id = sid;
        receiver_id = rid;
        type = 1;
    }

    // default constructor for generating txn and block events
    // type 2 block create event
    // type 4 Txn create event

    event(int gen_type,int rid,double at_time, int id = 0){
        type = gen_type;
        time = at_time;
        receiver_id = rid;
        this->id = id;
    }

    // comparator in the main file itself

};
struct Compare_event {
    bool operator()(event const& p1, event const& p2)
    {
       
        return p1.time > p2.time;
    }
};
/*
bool operator<(const event& a ,const event& b){


    return a.time >  b.time;   // gives min priority queue
    
}
*/

class Node{

public :

    int id;
    int coins; // no of bitcoins owned so far                                      // konda : no of bitcoins should be double
    bool slow;
    int num_nodes; // no. of nodes in the network
    double next_block_time;
    int  num_generated; // Added temporarily to ensure each node generates at max one block.


    vector<Node*> peers;
    vector<double> latency;

    unordered_set<string> all_transactions;
    unordered_set<string> transaction_pool;
    Block_node* genesis_block;
    Block_node* latest_block; // last block of the current longest chain

    unordered_set<int> received_blocks;
    unordered_set<int> received_txn;

    Node (int new_id, bool is_slow, Block genesis, int n_nodes, priority_queue<event, vector<event>, Compare_event> & pq, int bcoin = 0){
        id = new_id;
        coins = bcoin;
        slow = is_slow;
        num_nodes = n_nodes;
        num_generated = 0;

        genesis_block = new Block_node(genesis.id, 0, genesis.transactions, vector<int>(n_nodes,50));
        latest_block = genesis_block;
        
        next_block_time  = exponential(BLOCK_INTERARRIVAL, 0);
        event ev(2,id,next_block_time);
        pq.push(ev);

    }

    // Method to generate new block from existing pool of transactions
    Block generate_block(){
        Block new_block = Block(latest_block->id, id, num_nodes);
    
        int size=1;

        unordered_set<string> temp_transaction_pool(transaction_pool);
        transaction_pool.clear();
        vector<int> temp_balances(latest_block->balances);

        for(auto txn_string: temp_transaction_pool){
            Txn txn = Txn(txn_string);

            if(temp_balances[txn.sender_id] < txn.amount || size == MAX_BLOCK_SIZE){
                transaction_pool.insert(txn_string);
                continue;
            }

            temp_balances[txn.sender_id] -= txn.amount;
            size+=1;
            new_block.transactions.insert(txn.to_string());
        }

        for(string txn_string: new_block.transactions){
            Txn txn = Txn(txn_string);
            temp_balances[txn.receiver_id] += txn.amount;
        }


        Block_node* new_block_node = latest_block->add_child(&new_block, temp_balances);

        latest_block = new_block_node;

        received_blocks.insert(new_block.id);

        return new_block;
    }

    Txn generate_transaction(int txn_id){
        //cout << "generate txn called by :" << this->id << endl;
        srand(txn_id);
        int receiver = rand()%num_nodes;
        //cout << "reached 0 " << endl;
        int amount = rand()%(latest_block->balances[id] + 1);
        //cout << "reached " << endl;
        Txn new_txn = Txn(txn_id, id, receiver, amount);

        received_txn.insert(txn_id);

        transaction_pool.insert(new_txn.to_string());
        all_transactions.insert(new_txn.to_string());

        return new_txn;
    }

    //Recieve block returns a bool representing success or failure
    int receive_block(priority_queue<event, vector<event>, Compare_event> & pq, Block &new_block, double at_time){
        if(received_blocks.find(new_block.id) != received_blocks.end()){
            return 1;
        }
        else{
            received_blocks.insert(new_block.id);
        }

        Block_node* parent_node = genesis_block->find(new_block.parent_id);
        if(parent_node == NULL){
            cout << "BLOCK: " << new_block.id << " WITH PARENT ID: " << new_block.parent_id << " DOES'NT EXIST in Node: " << id << "\n";
            return -1;
        }
        if(parent_node->id != latest_block->id){
            unordered_set<string> trans_till_now;
            Block_node* node_ptr = parent_node;

            while(node_ptr != genesis_block){
                for(string txn_string: node_ptr->transactions){
                    Txn txn = Txn(txn_string);
                    trans_till_now.insert(txn.to_string());
                }
                node_ptr = node_ptr->parent;
            }

            
            vector<int> temp_balances(parent_node->balances);
            for(string txn_string: new_block.transactions){
                Txn txn = Txn(txn_string);
                if(txn.coinbase) continue;
                
                if(all_transactions.find(txn_string) == all_transactions.end()){
                    cout << "ERROR: transaction id: " << txn.txn_id << "is in block id: " << new_block.id << " with parent id: " << new_block.parent_id << " but not received by the node id: " << id << endl;
                    cout << "transaction id: " << txn.txn_id << " in Block id: " << new_block.id << " parent id: " << new_block.parent_id << endl;
                    return -1;
                }
                else if(trans_till_now.find(txn_string) != trans_till_now.end()){
                    cout << "PREVIOUSLY USED TRANSACTION USED AGAIN" << endl;
                    cout << "transaction id: " << txn.txn_id << " in Block id: " << new_block.id << " parent id: " << new_block.parent_id << endl;
                    return -1;
                }

                temp_balances[txn.sender_id] -= txn.amount;
                if(temp_balances[txn.sender_id] < 0){
                    cout << "DOUBLE SPEND" << endl;
                    cout << "transaction id: " << txn.txn_id << " in Block id: " << new_block.id << " parent id: " << new_block.parent_id << endl;
                    return -1;
                }
            }

            for(string txn_string: new_block.transactions){
                Txn txn = Txn(txn_string);
                temp_balances[txn.receiver_id] += txn.amount;
            }
            
            parent_node->add_child(&new_block,temp_balances);

            if(parent_node->length == latest_block->length){
                transaction_pool.clear();

                for(auto txn_string: all_transactions){

                    if(trans_till_now.find(txn_string) == trans_till_now.end() && new_block.transactions.find(txn_string) == new_block.transactions.end()){
                        transaction_pool.insert(txn_string);
                    }

                }
            }
            

        }
        else{
            vector<int> temp_balances(parent_node->balances);

            for(string txn_string: new_block.transactions){
                Txn txn = Txn(txn_string);

                if(txn.coinbase) continue;

                if(all_transactions.find(txn_string) == all_transactions.end()){
                    cout << "ERROR: transaction id: " << txn.txn_id << "is in block id: " << new_block.id << " with parent id: " << new_block.parent_id << " but not received by the node id: " << id << endl;
                    cout << "transaction id: " << txn.txn_id << " in Block id: " << new_block.id << " parent id: " << new_block.parent_id << endl;
                    return -1;
                }
                else if(transaction_pool.find(txn.to_string()) == transaction_pool.end()){
                    cout << "PREVIOUSLY USED TRANSACTION USED AGAIN" << endl;
                    cout << "transaction id: " << txn.txn_id << " in Block id: " << new_block.id << " parent id: " << new_block.parent_id << endl;
                    return -1;
                }

                temp_balances[txn.sender_id] -= txn.amount;
                if(temp_balances[txn.sender_id] < 0){
                    cout << "DOUBLE SPEND" << endl;
                    cout << "transaction id: " << txn.txn_id << " in Block id: " << new_block.id << " parent id: " << new_block.parent_id << endl;
                    return -1;
                }
            }

            for(string txn_string: new_block.transactions){
                Txn txn = Txn(txn_string);
                transaction_pool.erase(txn.to_string());
                temp_balances[txn.receiver_id] += txn.amount;
            }

            Block_node* new_block_node  = parent_node->add_child(&new_block, temp_balances);
            latest_block = new_block_node;
            if(num_generated < MAX_BLOCKS_GEN){
                next_block_time  = exponential(BLOCK_INTERARRIVAL, at_time);
                event ev(2,id,next_block_time);
                pq.push(ev);
            }

            return 0;
        }        
        return -1;
    }

    double network_delay(int i,int n_txn){

            double c = 100; // Mbps

            if(this->slow || peers[i]->slow) c = 5;

            double rho = this->latency[i];  // this is in milliseconds
            double bsize = 1+ n_txn;  // bsize KB; bytes not bits;
            default_random_engine generator(rand());
            exponential_distribution<double> expo(0.096/c);
            double d = expo(generator);
            return (rho/1000) + (bsize*8/(c*1000))+d;

    }

    void propagate_block(priority_queue<event, vector<event>, Compare_event> & pq, int sid, Block &block, double at_time){

        for(int i = 0;i<peers.size();i++){

            if(peers[i]->id != sid){
                //konda :  here ideally should decide the future time based on link latency 


                double nxt_time = network_delay(i,block.transactions.size());
                event future_event(block,this->id,peers[i]->id,at_time + nxt_time);
                pq.push(future_event);
            }

        }

    }

    int receive_transaction(Txn txn){
        if(received_txn.find(txn.txn_id) != received_txn.end()){
            return 1;
        }
        else{
            received_txn.insert(txn.txn_id);
        }
        
        if(latest_block->balances[txn.sender_id] >= txn.amount){
            transaction_pool.insert(txn.to_string());
            all_transactions.insert(txn.to_string());
            return 0;
        }
        return -1;
    }

    void propagate_transaction(priority_queue<event,vector<event>,Compare_event> & pq, int sid, Txn txn, double at_time){
        for(int i = 0;i<peers.size();i++){

            if(peers[i]->id != sid){
                //konda :  here ideally should decide the future time based on link latency 
                double nxt_time = network_delay(i,1);   // txn is 1 kiloByte
                event future_event(txn,this->id,peers[i]->id,at_time+nxt_time);
                pq.push(future_event);
            }

        }
    }



    void event_handler(priority_queue<event,vector<event>,Compare_event> & pq,event & cur,double at_time){

       
        // konda : this function is called by the reciever

        if (this->id != cur.receiver_id){
            cout << "event handler called by wrong peer node" << endl;
            return;
        }

        // konda : should perform txn check or block check based on type

        // konda : should check whether the block or txn already recieved by thisnode itself
        // konda : in short, every unique block should be forwarded only once. May recieve the same block from many other peers!
        // konda : better cache few distinct recently heard blocks


        if (cur.type == 1){
            //receive block event
            int ret = receive_block(pq, cur.B, at_time);
            if(ret == 0){
                //Forwarding to peers after receiving
                propagate_block(pq, cur.sender_id, cur.B, at_time);
            }
            else if(ret == -1){
                cout << "ERROR: faulty block: " << cur.B.id << " Message by Node number: " << this->id << endl;
            }
        }
        else if(cur.type == 2){
            //generate block event
            if(at_time != next_block_time || num_generated >= MAX_BLOCKS_GEN) return;

            Block new_block = generate_block();
            num_generated++;

            next_block_time  = exponential(BLOCK_INTERARRIVAL, at_time);
            event ev(2,id,next_block_time);
            pq.push(ev);

            propagate_block(pq, -1, new_block, at_time);
        }
        else if(cur.type == 3){
            //Transaction forwarding
            int ret = receive_transaction(cur.trans);
            if(ret == 0){
                propagate_transaction(pq, cur.sender_id,cur.trans, at_time);
            }
            else if(ret == -1){
                cout << "Faulty transaction: " << cur.trans.to_string() << " Message by Node number: " << this->id << endl; 
            }
        }
        else if(cur.type == 4){
            // generate transaction
           // cout << "event handler trigger by : " << this->id << endl;
        //    srand(at_time);
        //    int txn_id = rand()%INT_MAX+1;
            Txn new_txn = generate_transaction(cur.id);
           // cout << "generated txn succesfully" << endl;

            propagate_transaction(pq, -1, new_txn, at_time);

        }


        




    }



};




