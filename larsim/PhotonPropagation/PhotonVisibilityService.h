////////////////////////////////////////////////////////////////////////
// \file PhotonVisibilityService.h
//
// \brief Service to report opdet visibility to different points in
//         the system
//
// \author bjpjones@mit.edu
//
////////////////////////////////////////////////////////////////////////
#ifndef PHOTONVISIBILITYSERVICE_H
#define PHOTONVISIBILITYSERVICE_H


#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art/Framework/Services/Registry/ServiceMacros.h"
#include "larsim/PhotonPropagation/IPhotonLibrary.h"
#include "larsim/Simulation/PhotonVoxels.h"
#include "larcorealg/Geometry/geo_vectors_utils.h" // geo::vect namespace
#include "larcoreobj/SimpleTypesAndConstants/geo_vectors.h" // geo::Point_t


class TF1;

///General LArSoft Utilities
namespace phot{
  
  class PhotonVisibilityService {
  public:
    
    ~PhotonVisibilityService();
    PhotonVisibilityService(fhicl::ParameterSet const& pset, art::ActivityRegistry& reg);
    
    void reconfigure(fhicl::ParameterSet const& p);
    
    double GetQuenchingFactor(double dQdx) const;
    
    static double DistanceToOpDet(          double const* xyz, unsigned int OpDet )
      { return DistanceToOpDetImpl(geo::vect::makeFromCoords<geo::Point_t>(xyz), OpDet); }
    static double SolidAngleFactor(         double const* xyz, unsigned int OpDet )
      { return SolidAngleFactorImpl(geo::vect::makeFromCoords<geo::Point_t>(xyz), OpDet); }
    
    float GetVisibility(                    double const* xyz, unsigned int OpChannel, bool wantReflected=false ) const
      { return doGetVisibility(geo::vect::makeFromCoords<geo::Point_t>(xyz), OpChannel, wantReflected); }

    float const* GetAllVisibilities( double const* xyz, bool wantReflected=false ) const
      { return doGetAllVisibilities(geo::vect::makeFromCoords<geo::Point_t>(xyz), wantReflected); }
    
    void LoadLibrary() const;
    void StoreLibrary();
    
    
    void StoreLightProd(    int  VoxID,  double  N );
    void RetrieveLightProd( int& VoxID,  double& N ) const;
    
    void SetLibraryEntry(  int VoxID, int OpChannel, float N, bool wantReflected=false );
    float GetLibraryEntry( int VoxID, int OpChannel, bool wantReflected=false ) const;
    float const* GetLibraryEntries( int VoxID, bool wantReflected=false ) const;

    float const* GetReflT0s( double const* xyz ) const
      { return doGetReflT0s(geo::vect::makeFromCoords<geo::Point_t>(xyz)); }
    void SetLibraryReflT0Entry( int VoxID, int OpChannel, float value );
    float const* GetLibraryReflT0Entries( int VoxID ) const;
    float GetLibraryReflT0Entry( int VoxID, int Channel ) const;
 
    const std::vector<float>* GetTimingPar( double const* xyz ) const
      {  return doGetTimingPar(geo::vect::makeFromCoords<geo::Point_t>(xyz)); }
    void SetLibraryTimingParEntry( int VoxID, int OpChannel, float value, size_t parnum );
    const std::vector<float>* GetLibraryTimingParEntries( int VoxID ) const;
    float GetLibraryTimingParEntry( int VoxID, int Channel, size_t npar ) const;

    TF1* GetTimingTF1( double const* xyz ) const
      { return doGetTimingTF1(geo::vect::makeFromCoords<geo::Point_t>(xyz)); }
    void SetLibraryTimingTF1Entry( int VoxID, int OpChannel, TF1 func );
    TF1* GetLibraryTimingTF1Entries( int VoxID ) const;
 
    void SetDirectLightPropFunctions(TF1 const* functions[8], double& d_break, double& d_max, double& tf1_sampling_factor) const;
    void SetReflectedCOLightPropFunctions(TF1 const* functions[5], double& t0_max, double& t0_break_point) const;
    void LoadTimingsForVUVPar(std::vector<double> v[9], double& step_size, double& max_d, double& vuv_vgroup_mean, double& vuv_vgroup_max, double& inflexion_point_distance) const;
    void LoadTimingsForVISPar(std::vector<double>& distances, std::vector<std::vector<double>>& cut_off, std::vector<std::vector<double>>& tau, double& vis_vmean, double& n_vis, double& n_vuv, double& plane_depth) const; 
    void LoadGHForVUVCorrection(std::vector<std::vector<double>>& v, double& w, double& h, double& r, int& op_det_type) const;
    void LoadParsForVISCorrection(std::vector<std::vector<double>>& v, double& plane_depth, double& w_cathode, double& h_cathode, std::vector<double>& cntr_cathode, double& w, double& h, double& r, int& op_det_type) const;
 
    bool IsBuildJob() const { return fLibraryBuildJob; }
    bool UseParameterization() const {return fParameterization;}
    bool StoreReflected() const { return fStoreReflected; }
    bool StoreReflT0() const { return fStoreReflT0; }
    bool IncludeParPropTime() const { return fParPropTime; }
    size_t ParPropTimeNpar() const { return fParPropTime_npar; }
    std::string ParPropTimeFormula() const { return fParPropTime_formula; }

    bool IncludePropTime() const { return fIncludePropTime; }
    bool UseNhitsModel() const { return fUseNhitsModel; }

    const sim::PhotonVoxelDef& GetVoxelDef() const {return fVoxelDef; }
    size_t NOpChannels() const;
    
  private:

    int    fCurrentVoxel;
    double fCurrentValue;
    // for c2: fCurrentReflValue is unused
    //double fCurrentReflValue;

    float  fXmin, fXmax;
    float  fYmin, fYmax;
    float  fZmin, fZmax;
    int    fNx, fNy, fNz;

    bool fUseCryoBoundary;
    
    bool                 fLibraryBuildJob;
    bool                 fDoNotLoadLibrary;
    bool                 fParameterization;
    bool                 fHybrid;
    bool                 fStoreReflected;
    bool                 fStoreReflT0;
    bool                 fIncludePropTime;
    bool                 fUseNhitsModel;

    bool                 fParPropTime;
    size_t               fParPropTime_npar;
    std::string		 fParPropTime_formula;
    int                  fParPropTime_MaxRange;
    bool                 fInterpolate;
    bool                 fReflectOverZeroX;

    TF1 *fparslogNorm = nullptr;
    TF1 *fparslogNorm_far = nullptr;
    TF1 *fparsMPV = nullptr;
    TF1 *fparsMPV_far = nullptr;
    TF1 *fparsWidth = nullptr;
    TF1 *fparsCte = nullptr;
    TF1 *fparsCte_far = nullptr;
    TF1 *fparsSlope = nullptr;
    double fD_break, fD_max, fTF1_sampling_factor;
    TF1 *fparslogNorm_refl = nullptr;
    TF1 *fparsMPV_refl = nullptr;
    TF1 *fparsWidth_refl = nullptr;
    TF1 *fparsCte_refl = nullptr;
    TF1 *fparsSlope_refl = nullptr;
    double fT0_max, fT0_break_point;

    //for vuv time parametrization
    std::vector<double> fDistances_all;
    std::vector<double> fNorm_over_entries;
    std::vector<double> fMpv;
    std::vector<double> fWidth;
    std::vector<double> fDistances;
    std::vector<double> fSlope; 
    std::vector<double> fExpo_over_Landau_norm[3];
    double fstep_size, fmax_d, fvuv_vgroup_mean, fvuv_vgroup_max, finflexion_point_distance;   
    // for vis time parameterisation (exists for SBND, DUNE-SP)  
    std::vector<double> fDistances_refl;
    std::vector<std::vector<double>> fCut_off; 
    std::vector<std::vector<double>> fTau;
    double fvis_vmean, fn_LAr_VUV, fn_LAr_vis;

    //for the semi-analytic vuv/direct light signal (number of hits) correction
    //parametrization exists for DUNE SP & DP and for SBN-like detectors (SBND, MicroBooNE, ICARUS)
    std::vector<std::vector<double> > fGH_PARS;
    // for the semi-analytic visible/reflection light hits correction
    // parameters exist for DUNE SP only currently
    std::vector<std::vector<double>> fVIS_PARS;
    double fPlane_Depth, fCATHODE_height, fCATHODE_width;
    std::vector<double> fCATHODE_centre;

    double fAPERTURE_height, fAPERTURE_width, fPMT_radius;
    int fOptical_Detector_Type;

    std::string          fLibraryFile;      
    mutable IPhotonLibrary* fTheLibrary;
    sim::PhotonVoxelDef  fVoxelDef;
    
    geo::Point_t LibLocation(const double * xyz) const
      { return geo::vect::makeFromCoords<geo::Point_t>(xyz); }
    geo::Point_t LibLocation(geo::Point_t const& p) const;
    
    // --- BEGIN Implementation functions --------------------------------------
    /// @name Implementation functions
    /// @{

    static double DistanceToOpDetImpl(geo::Point_t const& p, unsigned int OpDet);
    
    static double SolidAngleFactorImpl(geo::Point_t const& p, unsigned int OpDet);
    
    float doGetVisibility
      (geo::Point_t const& p, unsigned int OpChannel, bool wantReflected = false)
      const;

    float const* doGetAllVisibilities
      (geo::Point_t const& p, bool wantReflected = false) const;
    
    float const* doGetReflT0s(geo::Point_t const& p) const;
    
    const std::vector<float>* doGetTimingPar(geo::Point_t const& p) const;
    
    TF1* doGetTimingTF1(geo::Point_t const& p) const;
    
    /// @}
    // --- END Implementation functions ----------------------------------------
    
    
  }; // class PhotonVisibilityService
} //namespace phot
DECLARE_ART_SERVICE(phot::PhotonVisibilityService, LEGACY)
#endif // UTIL_DETECTOR_PROPERTIES_H
