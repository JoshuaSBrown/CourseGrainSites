#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "cluster.h"

using namespace std;

#ifdef _E_
#define Err 1
#else
#define Err 0
#endif

static int clusterIdCounter = 0;
static int thresh = 0;

int setThresh(int n){
    if(n>0){
        thresh=n;
        if(Err==1) cout<<"Thresh hold is: "<<thresh<<endl;
        return 1;
    }else{
        if(Err==1) cerr<<"ERROR in setThresh"<<endl;
        return 0;
    }
}

cluster::cluster(int siteId1,int * neighId1, double * neighRates1, int sizen1, int visitFreq1, int siteId2, int * neighId2, double * neighRates2, int sizen2, int visitFreq2){
	cout<<"A"<<endl;
	clusterId=clusterIdCounter;
	clusterIdCounter++;
	site  * tmp=new site(siteId1,neighRates1,neighId1,sizen1,visitFreq1);
	site * tmp2= new site(siteId2,neighRates2,neighId2,sizen2,visitFreq2);
	cout<<"B"<<endl;
	if(potentialCluster(tmp,tmp2)){
			siteInCluster[0]= tmp;
			siteInCluster[1]= tmp2;
                        }
}

cluster::~cluster(){
}

int cluster::getClusterId(){
	return clusterId;
}

site ** cluster::getSitesInCluster(){
	return siteInCluster;
}

/*
int testCluster(){
	printf("potentialCluster(10,-10,10): (-1) %d\n",potentialCluster(10,-10));
	printf("potentialCluster(10,6,8): (0) %d\n",potentialCluster(10,6,8));
	printf("potentailCluster(10,10,5): (1) %d\n",potentialCluster(10,10,5));
	printf("clusterOrSite(-1,-1): (1) %d\n",clusterOrSite(-1,-1));
	printf("clusterOrSite(-10,2): (-1) %d\n",clusterOrSite(-10,2));
	printf("clusterOrSite(1,-1): (2) %d\n",clusterOrSite(1,-1));
	printf("clusterOrSite(1,1): (3) %d\n",clusterOrSite(1,1));
	return 0;
}

*/

int cluster::potentialCluster(site * site1, site * site2){
	if(site1->visitFreq<0||site2->visitFreq<0||thresh<0){
		fprintf(stderr,"ERROR in potentialCluster\n");//find a way to turn off in preprocessor
		return -1;
	}
	if((site1->visitFreq > thresh) && (site2->visitFreq > thresh))
		return 1;
	return 0;
}

int cluster::clusterOrSite(int clusterId1, int clusterId2){
	if(clusterId1<-1||clusterId2<-1)
		return -1;
	if(clusterId1 == -1 && clusterId2 == -1)
		return 1;
	if(clusterId1 == -1 || clusterId2 == -1)
		return 2;
	return 3;
}

int cluster::neighSiteCluster(site * site1, site * site2, int * neighCluster){
	int i=0, j=0;
	if(site1 ==NULL ||site2==NULL||neighCluster==NULL){
		if(Err) cerr<<"ERROR in neighSiteCluster"<<endl;
		return -1;
	}
	while(site1->neighIds[i]!=-1){ //Use -1 as a terminator
		neighCluster[i]=site1->neighIds[i];
	}
	while(site2->neighIds[j]!=-1){
		neighCluster[i+j]=site2->neighIds[j];
	}
	return 1;
}
