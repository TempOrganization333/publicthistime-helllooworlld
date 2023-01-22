#include <iostream>
bool is_prime(int n){
    if (n==2)
        return true;
    
    for(int i = 2; i<n; i++){
        if(n%i==0){
            return false;
        }
    }
    return true;
}

std::string plus_one(std::string a){
    return a + "1";
}


// int main(){
//     std::cout<<"Hello World.\n";
//     int x;
//     std::cin>>x;
//     std::cout<<is_prime(x);
// }
