#include <set>

#include "kmc_dynamic_topology.hpp" 

using namespace std;

namespace kmccoarsegrain {

  void KMC_Dynamic_Topology::setRandomSeed(const unsigned long seed) {
    /*      if (topology_features_.size() != 0) {
            throw runtime_error(
            "For the random seed to have an affect, it must be "
            "set before initializeSystem is called");
            }*/
    seed_ = seed;
    seed_set_ = true;
  }

  void KMC_Dynamic_Topology::setRates(unordered_map<int,unordered_map<int,double>> & rates){
    rates_ = &rates;
  }

  bool KMC_Dynamic_Topology::siteExist(int siteId){
    return sites_.exist(siteId);
  }

  KMC_Site & KMC_Dynamic_Topology::getKMC_Site(const int siteId){
    return sites_.getKMC_Site(siteId);
  }

  void KMC_Dynamic_Topology::setSitesClusterId(int siteId, int clusterId){
    sites_.getKMC_Site(siteId).setClusterId(clusterId);
  }

  KMC_Cluster & KMC_Dynamic_Topology::getKMC_Cluster(const int clusterId){
    return clusters_.getKMC_Cluster(clusterId);
  }

  void KMC_Dynamic_Topology::mergeClusters(const int clusterId1, const int clusterId2){
    clusters_.getKMC_Cluster(clusterId1).migrateSitesFrom(clusters_.getKMC_Cluster(clusterId2));
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
    return sites_.partOfCluster(siteId);
  }

  double KMC_Dynamic_Topology::getFastestRateOffSite(const int siteId){
    return sites_.getFastestRateOffSite(siteId);
  }

  int KMC_Dynamic_Topology::getClusterIdOfSite(int siteId){
    assert(sites_.exist(siteId) && "Site with provided id does not exist within the topology, perhaps it has not yet been created.");
    return sites_.getClusterIdOfSite(siteId); 
  }

  int KMC_Dynamic_Topology::getVisitFrequencyOfSite(int siteId){
    if(sites_.exist(siteId)==false){                                           
      throw invalid_argument("Site is not stored in the coarse grained system you"
          " cannot retrieve it's visit frequency.");                            
    }                                                                           

    int visits = sites_.getKMC_Site(siteId).getVisitFrequency();               
    if(sites_.partOfCluster(siteId)){                                          
      int cluster_id = sites_.getClusterIdOfSite(siteId);                      
      visits += clusters_.getKMC_Cluster(cluster_id).getVisitFrequency(siteId);
    }                                                                           
    return visits;     
  }

  void KMC_Dynamic_Topology::addKMC_Cluster(KMC_Cluster & cluster){
    if(seed_set_) {
      cluster.setRandomSeed(seed_);
      ++seed_;
    }
    clusters_.addKMC_Cluster(cluster);
    vector<int> siteIds = cluster.getSiteIdsInCluster();
    for( int & siteId : siteIds) {
      sites_.setClusterId(siteId,cluster.getId());
      features[siteId].feature = returnCluster;
    }
  }
  unordered_map<int,int> KMC_Dynamic_Topology::getClustersOfSites(const vector<int> & siteIds){
    unordered_map<int,int> sites_and_clusters;                                  
    for(auto siteId : siteIds){                                                 
      if(sites_.partOfCluster(siteId)){                                        
        sites_and_clusters[siteId]= sites_.getClusterIdOfSite(siteId);         
      }else{                                                                    
        sites_and_clusters[siteId]= constants::unassignedId;                    
      }                                                                         
    }                                                                           
    return sites_and_clusters;                                                  
  }

  KMC_TopologyFeature * returnSite(KMC_Dynamic_Topology & dyn_top,int siteId){ 
    //cout << "Getting site instead of creating one " << endl;
    return &(dyn_top.sites_.getKMC_Site(siteId));
  }

  KMC_TopologyFeature * returnCluster(KMC_Dynamic_Topology & dyn_top,int siteId){  
    //cout << "Getting cluster" << endl;                                          
    //cout << "Getting cluster " << dyn_top.sites_.getKMC_Site(siteId).getClusterId() << endl;
    return &(dyn_top.clusters_.getKMC_Cluster(dyn_top.sites_.getKMC_Site(siteId).getClusterId()));  
  }

  KMC_TopologyFeature * createSite(KMC_Dynamic_Topology & dyn_top,int siteId){
    KMC_Site & site = dyn_top.sites_.createKMC_Site(siteId);                    
    dyn_top.features[siteId].feature = &returnSite;               
    site.setRatesToNeighbors(&((*dyn_top.rates_)[siteId]));                        
    if (dyn_top.seed_set_) {                                                       
      site.setRandomSeed(dyn_top.seed_);                                           
      ++dyn_top.seed_;                                                             
    }                                                                           
    return &site;                                                               
  }    

  double KMC_Dynamic_Topology::getTimeConstantFromSitesToNeighbors(
      const vector<int> & siteIds) const {

    LOG("Get the minimum time constant", 1);
    set<int> internalSiteIds(siteIds.begin(), siteIds.end());

    double sumRates = 0.0;
    for (const int & siteId : siteIds) {
      vector<int> neighborSiteIds = sites_.getSiteIdsOfNeighbors(siteId);
      for (const int & neighId : neighborSiteIds) {
        if (!internalSiteIds.count(neighId)) {
          sumRates+= sites_.getRateToNeighborOfSite(siteId,neighId);
        }
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
      int clusterId = sites_.getClusterIdOfSite(siteId);                         
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
