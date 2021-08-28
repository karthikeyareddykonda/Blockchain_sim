#include "classes.cpp"
#include <random>

bool operator<(const event& a ,const event& b){


    return a.time >  b.time ;   // gives min priority queue
    
}

double poisson(double mean,double uni){
    return -mean*log(1-uni);
}


int main(){

    //cout << "okay" << endl;


    int n = 50 ; float z = 50 ;

    vector<Node> peers ;
    for(int i =0;i<n;i++){

        peers.push_back(Node(i));
        // should take care of being slow or fast

    }


    srand(time(0));

//-------------- sample for poisson random variable generation -------------

    default_random_engine generator(rand());
    uniform_real_distribution<double> uniform(0.0,1.0);
    

    
    double uni = uniform(generator);
    cout << "uniform : " <<  uni << " poisson : " << poisson(5 , uni) <<  endl;


















}