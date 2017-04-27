#include <iostream>

#include "cluster.h"

using namespace std;

#ifdef _E_
#define Err 1
#else
#define Err 0
#endif
/*
int testCluster(cluster * c){
	printf("potentialCluster(10,-10,10): (-1) %d\n",c->potentialCluster(10,-10));
	printf("potentialCluster(10,6,8): (0) %d\n",c->potentialCluster(10,6,8));
	printf("potentailCluster(10,10,5): (1) %d\n",c->potentialCluster(10,10,5));
	printf("clusterOrSite(-1,-1): (1) %d\n",c->clusterOrSite(-1,-1));
	printf("clusterOrSite(-10,2): (-1) %d\n",c->clusterOrSite(-10,2));
	printf("clusterOrSite(1,-1): (2) %d\n",c->clusterOrSite(1,-1));
	printf("clusterOrSite(1,1): (3) %d\n",c->clusterOrSite(1,1));
	return 0;
}
*/
int main(void){
    if(Err==1) cout<<"Yes"<<endl;
    else cout<<"No"<<endl;
    setThresh(10);
    setThresh(-20);
    int siteId1=1;
    int neighId1[]={2,-1};
    double neighRates[]={1.0,1.0};
    int size=2;
    int visitFreq=11;
    int siteId2=2;
    int neighId2[]={1,-1};
    cluster test = cluster(siteId1,neighId1,neighRates,size,visitFreq,siteId2,neighId2,neighRates,size,visitFreq);
    cout<<test.getClusterId()<<endl;
   //doesnt work, might not be needed, think about how to access clusters, gui? interface? direct pointers? should cluster still be a class?
   // site ** sites=test.getSitesInCluster();
   // cout<<(*sites)->siteId<<endl;
   // cout<<site[1]->siteId<<endl;
    return 0;
}