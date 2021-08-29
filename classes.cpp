#include<bits/stdc++.h>

using namespace std;

#define MAX_BLOCK_SIZE 1024

class Txn{

public:

    string s;
    int sender_id;
    int receiver_id;
    double amount;
    bool coinbase;

    Txn (int sid,int rid,double trans_amount, bool coinbase = false){
        sender_id = sid;
        receiver_id = rid;
        amount = trans_amount;
        this->coinbase = coinbase;

        //generate appropriate string and store in s (separate cases for with and without coinbase)
    }

    Txn (string s){
        // parse the string etc
        sender_id = -1;
    }

    // empty constructor, required for passing into other class constructor
    Txn (){

        sender_id = -1 ;
    }

    string to_string(){
        return s;
    }




};

class Block{

public:

    uint32_t id;

    vector<string> transactions; 

    vector<int> balances;

    uint32_t parent_id;


    // normal block constructor
    Block(uint32_t parent_id, int miner_id){

        // Assigning address as id
        int new_id = rand()%INT_MAX;
        id = new_id;
        this->parent_id = parent_id;

        // Adding coinbase transaction
        Txn coinbase = Txn(-1, miner_id, 50, true);

    }

    // default constructor, should be genysys block

    Block(){
        parent_id = -1 ;
        id = 0;
    }





};


class Block_node{

public:

    uint32_t id;
    vector<string> transactions; 
    vector<int> balances;
    vector<Block_node*> children;
    int length;         // distance from the genesis block_node

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



class Node{

public :

    int id;
    int coins ; //   no of bitcoins owned so far 
    bool slow;

    vector<Node*> peers;
    vector<double> latency;

    queue<Txn> transaction_pool;
    Block_node* genesis_block;
    Block_node* latest_block; // last block of the current longest chain

    priority_queue<pair<string,int>>* event_queue; // To be assigned value in the contructor
    unordered_set<uint32_t> received_blocks;

    Node (int new_id, bool is_slow, int bcoin = 0){
        // Assigning address as id
        id = new_id;
        coins = bcoin;
        slow = is_slow;
    }

    // Method to generate new block from existing pool of transactions
    Block generate_block(){
        Block new_block = Block(latest_block->id, id);
    
        int size=1;

        while(!transaction_pool.empty()){
            Txn txn = transaction_pool.front();
            transaction_pool.pop();
            size+=1;
            new_block.transactions.push_back(txn.to_string());

            if(size + 1 > MAX_BLOCK_SIZE){
                break;
            }
        }

        return new_block;
    }

    // broadcast function // should be loopless // should also check validity // should also return information for future events
    // PLAN: set up recieve events for peers (based on latency etc), they will do the same when they recieve the block, every block first checks whether the block has already been "recieved" (visited in the bfs)
    bool broadcast_block(){
        return true ;
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

            // transmits the block to peers (pushes respective recieve events in the event_queue)


            return true;
        }
        return false;
    }



};



class event{


    public :
    int type; // type of event, send to peer, recieve, 
    double time ; // the time at which event to be happened
    Txn trans;
    Block B ;
    // possible extra information regarding event, sender reciever, txn, block

    

    // constructor

    // the recieving txn constructor, node should trigger check and broadcast
    event (Txn tn, double at_time){
        trans = tn ;
        time = at_time ;
    }

    event (Block cur_block, double at_time){
        B = cur_block ;
        time = at_time;
    }

    // comparator in the main file itself

};
