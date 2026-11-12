// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef JETANALYZER_H
#define JETANALYZER_H

// General C++ libraries
#include <iostream>
#include <iomanip>
#include <string>

// ROOT headers
#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
// #include <Math/Vector3D.h> // for 3D vector
// #include <Math/Vector4D.h> // for 4-momentum

// Fun4All general
#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllReturnCodes.h>
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

// GL1
#include <ffarawobjects/Gl1Packetv3.h>

// Vertex
#include <globalvertex/GlobalVertex.h>
#include <globalvertex/GlobalVertexMap.h>

// Jet
#include <jetbase/JetContainerv1.h>
#include <jetbase/Jetv2.h>

class PHCompositeNode;

class JetAnalyzer : public SubsysReco
{
 public:
  
  JetAnalyzer(const std::string &name = "JetAnalyzer");

  ~JetAnalyzer() override;

  ////////////////////////////////////////////////////////////
  // Fun4All functions
  ////////////////////////////////////////////////////////////
  int Init(PHCompositeNode *topNode) override;

  int InitRun(PHCompositeNode *topNode) override;

  int process_event(PHCompositeNode *topNode) override;

  /// Clean up internals after each event.
  int ResetEvent(PHCompositeNode *topNode) override;

  /// Called at the end of each run.
  int EndRun(const int runnumber) override;

  /// Called at the end of all processing.
  int End(PHCompositeNode *topNode) override;

  /// Reset
  int Reset(PHCompositeNode * /*topNode*/) override;

  void Print(const std::string &what = "ALL") const override;

  ////////////////////////////////////////////////////////////
  // Getter/Setter
  ////////////////////////////////////////////////////////////
  int GetEventId() const { return event_id_; }
  std::string GetOutput() const { return output_; }
  int GetRun() const { return run_; }
    
  void SetOutput( std::string output ){ output_ = output;};
  void SetRun( int num ){ run_ = num;};
  
 private:

  ////////////////////////////////////////////////////////////
  // Output parameters/objects
  ////////////////////////////////////////////////////////////
  std::string output_ = "JetAnalyzer.root";
  TFile* tf_;

  ////////////////////////////////////////////////////////////
  // TTree
  ////////////////////////////////////////////////////////////
  TTree* tr_jet_ = nullptr;
  TTree* tr_event_ = nullptr;

  ////////////////////////////////////////////////////////////
  // Parameters for TTree
  ////////////////////////////////////////////////////////////
  int run_ = 0; // needed?
  // Event parameters
  int event_id_ = -1;
  int jet_num_ = - 1;
  double vertex_z_ = -9999.9; // vertex position in z in cm

  // GL1 variables
  std::string gl1_packet_name_ = "14001"; // "GL1Packet"; // GL1Packet is an old name. Forget about it.
  std::vector < int > trigger_vector_;
  std::vector < int > live_vector_;
  std::vector < int > scaled_vector_;
  std::vector < int > gtm_busy_vector_;

  //ROOT::Math::XYZVector vertex_mbd_; // Mathmatical 3D vector is good for vertex, but it's commented out for simlisity

  // Jet parameters
  //ROOT::Math::PtEtaPhiEVector jet_;
  double jet_pt_ = 0.0; // Jet transverse momentum in GeV/c
  
  ////////////////////////////////////////////////////////////
  // Objects to be output
  ////////////////////////////////////////////////////////////
  TH1D* hist_jet_pt_ = nullptr; // jet pT distribution in GeV/c

  ////////////////////////////////////////////////////////////
  // Functions
  ////////////////////////////////////////////////////////////
  std::vector < int > GetFlagVector( uint64_t arg );
  int ProcessGl1( PHCompositeNode* topNode );
  int MbdVertex(PHCompositeNode *topNode );
};

#endif // JETANALYZER_H
