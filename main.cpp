#include<bits/stdc++.h>
#include "classes.cpp"

using namespace std;

void make_graph(Node network[]){
    
}

int main() {
    //Reading input for number of nodes
    int n; cin >> n;
    int slow_percentage; cin >> slow_percentage;

    Node* network = new Node[n];

    for(int i = 0; i < n;i++){
        network[i] = Node();
    }

    make_graph(network);
    
}