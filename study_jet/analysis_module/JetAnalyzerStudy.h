// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef JETANALYZERSTUDY_H
#define JETANALYZERSTUDY_H

// General C++ libraries
#include <iostream>
#include <iomanip>
#include <string>

// ROOT headers
#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>

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

class JetAnalyzerStudy : public SubsysReco
{
 public:
  
  JetAnalyzerStudy(const std::string &name = "JetAnalyzerStudy");

  ~JetAnalyzerStudy() override;

  ////////////////////////////////////////////////////////////
  // Fun4All functions
  ////////////////////////////////////////////////////////////
  int Init(PHCompositeNode *topNode) override;
  int InitRun(PHCompositeNode *topNode) override;
  int process_event(PHCompositeNode *topNode) override;
  int ResetEvent(PHCompositeNode *topNode) override;
  int EndRun(const int runnumber) override;
  int End(PHCompositeNode *topNode) override;
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
  std::string output_ = "JetAnalyzerStudy.root";
  TFile* tf_ = nullptr;

  ////////////////////////////////////////////////////////////
  // TTree
  ////////////////////////////////////////////////////////////
  TTree* tr_jet_ = nullptr;
  TTree* tr_event_ = nullptr;

  ////////////////////////////////////////////////////////////
  // Parameters for TTree
  ////////////////////////////////////////////////////////////
  int run_ = 0; 
  int event_id_ = -1;
  int jet_num_ = - 1;
  int jet_id_ = -1;
  double vertex_z_ = -9999.9; 

  // GL1 variables
  std::string gl1_packet_name_ = "14001"; // "GL1Packet"; // GL1Packet is an old name. Forget about it.
  std::vector < int > trigger_vector_;
  std::vector < int > live_vector_;
  std::vector < int > scaled_vector_;
  std::vector < int > gtm_busy_vector_;

  // Jet parameters
  double jet_pt_ = 0.0; 
  double jet_eta_ = 0.0;
  double jet_phi_ = 0.0;
  double jet_e_ = 0.0;
  int jet_nconst_ = 0;

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

#endif // JETANALYZERSTUDY_H
