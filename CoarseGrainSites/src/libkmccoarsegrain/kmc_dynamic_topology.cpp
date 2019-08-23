#include <algorithm>
#include <set>

#include "kmc_dynamic_topology.hpp" 

using namespace std;

namespace kmccoarsegrain {

  void KMC_Dynamic_Topology::setRandomSeed(const unsigned long seed) {
    
    seed_ = seed;
    seed_set_ = true;
  }

  unordered_map<int,unordered_map<int,double>> 
    KMC_Dynamic_Topology::getExternallyConnectedRates(vector<int> site_ids){

      sort(site_ids.begin(),site_ids.end());
      unordered_map<int,unordered_map<int,double>> external_rates;
      for( const int & siteId : site_ids){
        for( const pair<int,double> & neigh_rate : rates_->at(siteId)){
          if(binary_search(site_ids.begin(),site_ids.end(),neigh_rate.first)){
            continue;
          }
          external_rates[siteId][neigh_rate.first] = neigh_rate.second;
        }
      }
      return external_rates;
    }

  void KMC_Dynamic_Topology::setRates(unordered_map<int,unordered_map<int,double>> & rates){
    rates_ = &rates;
  }

  // This should not be erroneous as long as the rates satisfy detailed balance
  bool KMC_Dynamic_Topology::siteExist(int siteId){
    return rates_->count(siteId);
  }

  void KMC_Dynamic_Topology::setSitesClusterId(int siteId, int clusterId){
    site_and_cluster_[siteId] = clusterId; 
  }

  KMC_Cluster & KMC_Dynamic_Topology::getKMC_Cluster(const int clusterId){
    return clusters_.getKMC_Cluster(clusterId);
  }

  void KMC_Dynamic_Topology::mergeClusters(const int clusterId1, const int clusterId2){
    clusters_.getKMC_Cluster(clusterId1).migrateSitesFrom(clusters_.getKMC_Cluster(clusterId2));
    vector<int> siteIds = clusters_.getKMC_Cluster(clusterId2).getSiteIdsInCluster();
    for(const int & siteId : siteIds){
      site_and_cluster_[siteId] = clusterId1; 
    }
    clusters_.erase(clusterId2);   
  }

  unordered_map<int,double> & KMC_Dynamic_Topology::getSiteRates(const int siteId){
    return (*rates_)[siteId];  
  }
  double KMC_Dynamic_Topology::getRate(const int siteId1, const int siteId2){
    return (*rates_)[siteId1][siteId2];
  }

  double KMC_Dynamic_Topology::getFastestRateOffCluster(const int clusterId){
    return clusters_.getFastestRateOffCluster(clusterId);
  }

  bool KMC_Dynamic_Topology::partOfCluster(const int siteId){
    return site_and_cluster_.count(siteId);
  }

  double KMC_Dynamic_Topology::getFastestRateOffSite(const int siteId){
    double fastest_rate = 0.0; 
    for ( pair<int, double> & neigh_rate : (*rates_)[siteId] ){
      if(neigh_rate.second>fastest_rate){
        fastest_rate = neigh_rate.second;
      }
    }
    return fastest_rate;
  }

  int KMC_Dynamic_Topology::getClusterIdOfSite(int siteId){
    return site_and_cluster_.at(siteId); 
  }

  void KMC_Dynamic_Topology::addKMC_Cluster(KMC_Cluster & cluster){
    if(seed_set_) {
      cluster.setRandomSeed(seed_);
      ++seed_;
    }
    clusters_.addKMC_Cluster(cluster);
    vector<int> siteIds = cluster.getSiteIdsInCluster();
    for( int & siteId : siteIds) {
      site_and_cluster_[siteId] = cluster.getId();  
    }
  }
  unordered_map<int,int> KMC_Dynamic_Topology::getClustersOfSites(const vector<int> & siteIds){
    unordered_map<int,int> sites_and_clusters;                                  
    for(auto siteId : siteIds){                                                 
      if(site_and_cluster_.count(siteId)){
        sites_and_clusters[siteId] = site_and_cluster_.at(siteId);         
      }else{
        sites_and_clusters[siteId]= constants::unassignedId; 
      }
    }                                                                           
    return sites_and_clusters;                                                  
  }
  
double KMC_Dynamic_Topology::getTimeConstantFromSitesToNeighbors(
    const vector<int> & siteIds) const {

  LOG("Get the minimum time constant", 1);
  set<int> internalSiteIds(siteIds.begin(), siteIds.end());

  double sumRates = 0.0;
  for (const int & siteId : siteIds) {
  for( const pair<int,double> & neigh_rate : rates->at(siteId) ){
      if (!internalSiteIds.count(neigh_rate.first)) {
        sumRates+= sites_.getRateToNeighborOfSite(siteId,neigh_rate.first);
    }
  }
  if (sumRates == 0.0){
    return 0.0;
  }
  return 1.0/sumRates;
}

unordered_map<int,vector<int>> KMC_Dynamic_Topology::getClusters(){
  return clusters_.getSiteIdsOfClusters();
}

unordered_map<int,double> KMC_Dynamic_Topology::getResolutionOfClusters(){
  return clusters_.getResolutionOfClusters();
}

unordered_map<int,double> KMC_Dynamic_Topology::getTimeIncrementOfClusters(){
  return clusters_.getTimeIncrementOfClusters();
}

int KMC_Dynamic_Topology::getFavoredClusterId(vector<int> siteIds){
  LOG("Getting the favored cluster Id", 1);                                     
  int favoredClusterId = constants::unassignedId;                               
  for (auto siteId : siteIds) {                                                 
    int clusterId = site_and_cluster_.at(siteId);                         
    if (favoredClusterId == constants::unassignedId) {                          
      favoredClusterId = clusterId;                                             
    } else if (clusterId != constants::unassignedId &&                          
        clusterId < favoredClusterId) {                                  
      favoredClusterId = clusterId;                                             
    }                                                                           
  }                                                                             
  return favoredClusterId;    
}
}
