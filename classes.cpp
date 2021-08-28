#include<bits/stdc++.h>

using namespace std;

#define MAX_BLOCK_SIZE 1024


class Node{

    int id;
    int coins ; //   no of bitcoins owned so far 
    bool heavy;

    vector<Node*> peers;
    vector<double> latency;

    queue<Txn> transaction_pool;
    Block_node* genesis_block;
    Block_node* latest_block; // last block of the current longest chain

    priority_queue<pair<string,int>>* event_queue; // To be assigned value in the contructor
    unordered_set<uint32_t> received_blocks;


public :

    Node (int new_id, int bcoin = 0){
        // Assigning address as id
        id = new_id;
        coins = bcoin;
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

    // broadcast function // should be loopless
    // PLAN: set up recieve events for peers (based on latency etc), they will do the same when they recieve the block, every block first checks whether the block has already been "recieved" (visited in the bfs)
    bool broadcast_block(){
        
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
        bool status  = parent_node->add_child(&new_block);
        if(status){
            // transmits the block to peers (pushes respective recieve events in the event_queue)
        }
        return status;
    }



};


class Block{

public:

    uint32_t id;

    vector<string> transactions; 

    vector<int> balances;

    uint32_t parent_id;


    // constructor
    Block(uint32_t parent_id, int miner_id){

        // Assigning address as id
        uint32_t new_id = reinterpret_cast<uint32_t>(this);
        id = new_id;
        this->parent_id = parent_id;

        // Adding coinbase transaction
        Txn coinbase = Txn(-1, miner_id, 50, true);

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
    bool add_child(Block* new_block){

        // Verifying transactions with current balances
        vector<int> temp_balances(balances);

        for(string txn_string: new_block->transactions){
            Txn txn = Txn(txn_string);
            temp_balances[txn.sender_id] -= txn.amount;
            if(temp_balances[txn.sender_id] < 0){
                return false;
            }
        }

        // Verification successful, making a new Block_node to add in the tree
        Block_node* new_block_node = new Block_node(new_block->id, this->length+1, new_block->transactions, temp_balances);
        children.push_back(new_block_node);

        return true;
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




class Txn{

public:

    string s;
    int sender_id;
    int receiver_id;
    double amount;
    bool coinbase;

    vector<Block> children;

    Txn (int sid,int rid,double trans_amount, bool coinbase = false){
        sender_id = sid;
        receiver_id = rid;
        amount = trans_amount;
        this->coinbase = coinbase;

        //generate appropriate string and store in s (separate cases for with and without coinbase)
    }

    Txn (string s){
        // parse the string etc
    }

    string to_string(){
        return s;
    }


};



class event{


    int type; // type of event, send to peer, recieve, 
    double time ; // the time at which event to be happened

    // possible extra information regarding event, sender reciever, txn, block

    public :

    // constructor 



    // should also implement the comparator, for priority queue





};
