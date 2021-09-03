#include<bits/stdc++.h>

using namespace std;

#define MAX_BLOCK_SIZE 1024

class Txn{

public:

    string s;
    int txn_id;
    int sender_id;
    int receiver_id;
    double amount;
    bool coinbase;

    Txn (int sid,int rid,double trans_amount, bool coinbase = false){
        txn_id = rand()%INT_MAX+1;
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

    vector<string> transactions; 
    vector<int> balances;



    // normal block constructor
    Block(int parent_id, int miner_id, int num_nodes){
        int new_id = rand()%INT_MAX+1;
        cout << "block created id : " << new_id << endl;
        
        id = new_id;
        this->parent_id = parent_id;

        // Adding coinbase transaction
        Txn coinbase = Txn(-1, miner_id, 50, true);
        transactions.push_back(coinbase.to_string());

        balances = vector<int>(num_nodes,0);

    }

    // default constructor

    Block(){
        parent_id = -1 ;
        id = 0;
    }





};


class Block_node{

public:

    int id;
    int length;         // distance from the genesis block_node

    vector<string> transactions; 
    vector<int> balances;
    vector<Block_node*> children;

    //TODO
    vector<int> counts;  //  to track the family tree counts , should use dfs/bfs

    // constructor
    Block_node(int id, int length, vector<string> transactions, vector<int> balances){
        this->id = id;
        this->length = length;
        this->transactions = transactions;
        this->balances = balances;
    }

    // adding of child (returns bool to indicate success)
    Block_node* add_child(Block* new_block){

        // Verifying transactions with current balances
        vector<int> temp_balances(balances);

        for(string txn_string: new_block->transactions){
            Txn txn = Txn(txn_string);

            if(txn.coinbase) continue;

            temp_balances[txn.sender_id] -= txn.amount;
            if(temp_balances[txn.sender_id] < 0){
                return NULL;
            }
        }

        // Verification successful, making a new Block_node to add in the tree
        Block_node* new_block_node = new Block_node(new_block->id, this->length+1, new_block->transactions, temp_balances);
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
                result = block->find(id);

                if(result != NULL) return result;
            }

            return result;
        }
    }

};

class event{


    public :
    int type; // type of event, block recieve or txn recieve 
    double time ; // the time at which event to be happened
    Txn trans;
    Block B ;
    int sender_id;
    int receiver_id ;
    // possible extra information regarding event, sender reciever, txn, block
    // here sender is the network sender , not the one in txn
    

    // constructor

    // the recieving txn constructor, node should trigger check and broadcast
    event (Txn tn,int sid, int rid ,double at_time){
        trans = tn ;
        time = at_time ;
        type = 0;
        sender_id = sid ;
        receiver_id = rid ;
    }

    event (Block cur_block,int sid,int rid, double at_time){
        B = cur_block ;
        time = at_time;
        sender_id = sid ;
        receiver_id = rid ;
        type = 1;
    }

    // comparator in the main file itself

};

bool operator<(const event& a ,const event& b){


    return a.time >  b.time ;   // gives min priority queue
    
}


class Node{

public :

    int id;
    int coins ; // no of bitcoins owned so far                                      // konda : no of bitcoins should be double
    bool slow;
    int num_nodes; // no. of nodes in the network

    vector<Node*> peers;
    vector<double> latency;


    queue<string> transaction_pool;
    Block_node* genesis_block;
    Block_node* latest_block; // last block of the current longest chain

    unordered_set<int> received_blocks;

    Node (int new_id, bool is_slow, Block genesis, int n_nodes, int bcoin = 0){
        id = new_id;
        coins = bcoin;
        slow = is_slow;
        num_nodes = n_nodes;
        genesis_block = new Block_node(genesis.id, 0, genesis.transactions, genesis.balances);
    }

    // Method to generate new block from existing pool of transactions
    Block generate_block(){
        Block new_block = Block(latest_block->id, id, num_nodes);
    
        int size=1;

        while(!transaction_pool.empty()){
            Txn txn = Txn(transaction_pool.front());
            transaction_pool.pop();
            size+=1;
            new_block.transactions.push_back(txn.to_string());

            if(size == MAX_BLOCK_SIZE){
                break;
            }
        }

        return new_block;
    }

    Txn generate_transaction(){
        int receiver = rand()%num_nodes;
        int amount = rand()%(latest_block->balances[id] + 1);
        Txn new_txn = Txn(id, receiver, amount);

        return new_txn;
    }

    // broadcast function // should be loopless // should also check validity // should also return information for future events
    // PLAN: set up recieve events for peers (based on latency etc), they will do the same when they recieve the block, every node first checks whether the block has already been "recieved" (visited in the bfs)
    bool broadcast_block(){
        return true;
    }

    //Recieve block returns a bool representing success or failure
    bool receive_block(Block &new_block){
        if(received_blocks.find(new_block.id) != received_blocks.end()){
            return true;
        }
        else{
            received_blocks.insert(new_block.id);
        }

        Block_node* parent_node = genesis_block->find(new_block.parent_id);
        if(parent_node == NULL){
            cout << "BLOCK WITH PARENT ID DOES'NT EXIST\n";
            return false;
        }

        // Verification in add_child
        Block_node* new_block_node  = parent_node->add_child(&new_block);
        if(new_block_node != NULL){
            // If new longest chain is found (or existing chain is being extended)
            // we replace the latest block
            if(new_block_node->length > latest_block->length){
                latest_block = new_block_node;
            }

            return true;
        }
        return false;
    }

    void propagate_block(priority_queue<event> & pq, int sid, Block &block, double at_time){

        for(int i = 0;i<peers.size();i++){

            if(peers[i]->id != sid){
                //konda :  here ideally should decide the future time based on link latency 
                event future_event(block,this->id,peers[i]->id,at_time+10);
                pq.push(future_event);
            }

        }

    }

    bool receive_transaction(Txn txn){
        if(latest_block->balances[txn.sender_id] >= txn.amount){
            transaction_pool.push(txn.to_string());
            return true;
        }
        return false;
    }

    void propagate_transaction(priority_queue<event> & pq, int sid, Txn txn, double at_time){
        for(int i = 0;i<peers.size();i++){

            if(peers[i]->id != sid){
                //konda :  here ideally should decide the future time based on link latency 
                event future_event(txn,this->id,peers[i]->id,at_time+10);
                pq.push(future_event);
            }

        }
    }



    void event_handler(priority_queue<event> & pq,event & cur,double at_time){

       
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

            if(receive_block(cur.B)){
                //Forwarding to peers after receiving
                propagate_block(pq, cur.sender_id, cur.B, at_time);
            }
            else{
                cout << "ERROR: faulty block, Message by Node number: " << this->id << endl;
            }
        }
        else if(cur.type == 2){
            //generate block event
            Block new_block = generate_block();

            propagate_block(pq, -1, new_block, at_time);
        }
        else if(cur.type == 3){
            //Transaction forwarding

            if(receive_transaction(cur.trans)){
                propagate_transaction(pq, cur.sender_id,cur.trans, at_time);
            }
            else{
                cout << "Faulty transaction: " << cur.trans.to_string() << " Message by Node number: " << this->id << endl; 
            }
        }
        else if(cur.type == 4){
            // generate transaction
            Txn new_txn = generate_transaction();

            propagate_transaction(pq, -1, new_txn, at_time);

        }


        




    }



};




