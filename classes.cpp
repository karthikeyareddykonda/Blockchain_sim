#include<bits/stdc++.h>

using namespace std;




class Node{

    int id ;
    int coins ; //   no of bitcoins owned so far 

    vector<Node> peers;
    vector<double> latency;


    public :
    Node (int new_id, int bcoin=0){
        id = new_id;
        coins = bcoin;
    }

    // Txn  validity checker function

    bool Txn_check(Txn transaction){

        return true;

    } 

    // Block chain tree, should be with genysys block in the begining




    //broadcast function // should be loopless




};


class Block{

    int block id;

    vector<Txn> transactions ; 

    int prev_hash;


    public :


    // constructor




    // Merkel tree maintaining


};

class Block_node{

    Block root ;

    vector<Block> children;
    vector<int> counts;  //  to track the family tree counts , should use dfs/bfs

    int long_ind;   // the index of children involving longest path
    public:

    // constructor



    // adding of child

};


class Block_chain{


    
    Block_node root;

    Block_node last_child ; // the child of longest chain. easy to start mining

    public :

    Block_chain (){
        // start with genesys block node
    }


    void print(){
        // should either print to terminal or 


    }

    void add(){

        // should take care of updating the longest chain, and also the last child
        // should do the check of hash matching, if not, then trace the entire tree for the matching and add as a child there


    }



}









class Txn{


    string s;
    int sender_id;
    int receiver_id;
    double coins_inp,coins_out;
    double mining_fee ;
    public :Block root ;

    vector<Block> children;by Nodes themselves
    Txn (string ts, int sid,int rid,double inp_coins,double out_coins){
        s = ts;
        sender_id = sid;
        receiver_id = rid;
        coins_inp = inp_coins;
        coins_out = out_coins;
        mining_fee = inp_coins - out_coins;
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
