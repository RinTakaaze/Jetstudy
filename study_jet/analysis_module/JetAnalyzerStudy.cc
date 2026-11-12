#include "JetAnalyzerStudy.h"

JetAnalyzerStudy::JetAnalyzerStudy(const std::string &name):
  SubsysReco(name)
{

}

JetAnalyzerStudy::~JetAnalyzerStudy()
{
}

int JetAnalyzerStudy::Init(PHCompositeNode *topNode)
{

  this->ResetEvent( topNode );
  tf_ = new TFile( output_.c_str(), "RECREATE" );

  ////////////////////////////////////////////////////////////
  // Jet-base TTree
  ////////////////////////////////////////////////////////////
  tr_jet_ = new TTree( "tr_jet", "Jet tree" );
  // numbers for each jets ////////////////////////
  tr_jet_->Branch( "event", &event_id_, "event/I" );
  tr_jet_->Branch( "jet_id", &jet_id_, "jet_id/I" );
  tr_jet_->Branch( "vertex_z", &vertex_z_, "vertex_z/D" );
  
  tr_jet_->Branch( "jet_pt", &jet_pt_, "jet_pt/D" );
  //added paramateres////
  tr_jet_->Branch( "jet_eta", &jet_eta_, "jet_eta/D" );
  tr_jet_->Branch( "jet_phi", &jet_phi_, "jet_phi/D" );
  tr_jet_->Branch( "jet_e", &jet_e_, "jet_e/D" );
  tr_jet_->Branch( "jet_nconst", &jet_nconst_, "jet_nconst/I" );   
  
  
  ////////////////////////////////////////////////////////////
  // Event-base ttree
  ////////////////////////////////////////////////////////////
  tr_event_ = new TTree( "tr_event", "Event-base TTree" );
  // numbers for each events ////////////////////////
  tr_event_->Branch( "run", &run_, "run/I" );
  tr_event_->Branch( "event", &event_id_, "event/I" );
  
  tr_event_->Branch( "trigger_vector", &trigger_vector_ );
  tr_event_->Branch( "live_vector", &live_vector_ );
  tr_event_->Branch( "scaled_vector", &scaled_vector_ );
  tr_event_->Branch( "vertex_z", &vertex_z_, "vertex_z/D" );
  tr_event_->Branch( "jet_num", &jet_num_, "jet_num/I" );
  
  ////////////////////////////////////////////////////////////
  // Histogram
  ////////////////////////////////////////////////////////////
  hist_jet_pt_ = new TH1D( "hist_jet_pt", "Jet p_{T}; p_{T} (GeV);Counts/GeV",
			   100, 0, 100 );
  
  return Fun4AllReturnCodes::EVENT_OK;
}

int JetAnalyzerStudy::InitRun(PHCompositeNode *topNode)
{

  return Fun4AllReturnCodes::EVENT_OK;
}

// This is the main analysis part
int JetAnalyzerStudy::process_event(PHCompositeNode *topNode)
{
  this->ResetEvent( topNode );
  ////////////////////////////////////////////////////////////////////////
  // Trigger
  ////////////////////////////////////////////////////////////////////////
  //this->ProcessGl1( topNode ); // Forget about GL1 analysis but just run this function
  int ret = this->ProcessGl1(topNode);
  if (ret != Fun4AllReturnCodes::EVENT_OK)
  return ret;

  ////////////////////////////////////////////////////////////////////////
  // Vertex
  ////////////////////////////////////////////////////////////////////////
  //this->MbdVertex( topNode ); // Forget about MBD vertex analysis but just run this function
  ret = this->MbdVertex(topNode);
  if (ret != 0)
  return Fun4AllReturnCodes::ABORTEVENT;

  ////////////////////////////////////////////////////////////////////////
  // Jet
  ////////////////////////////////////////////////////////////////////////
  // Preparation: Get tje jet node object
  std::string jet_node_name = "AntiKt_unsubtracted_r04";
  auto node_jet = findNode::getClass<JetContainerv1>(topNode, jet_node_name );
  if( !node_jet )
    {
      std::cerr << jet_node_name << " is not found." << std::endl;
      return  Fun4AllReturnCodes::ABORTEVENT;
    }

  // Jet analysis //////////////////////////////////////////////////////////////////
  // Loop over all jets in an event
  for( const auto jet : *node_jet ) // auto jet -> Jet class 
    {
      jet_id_ = jet_num_;

      jet_pt_ = jet->get_pt();
      jet_eta_ = jet->get_eta();
      jet_phi_ = jet->get_phi();
      jet_e_   = jet->get_e();
      jet_nconst_ = jet->size_comp();

      hist_jet_pt_->Fill( jet_pt_ );
      tr_jet_->Fill();

      jet_num_ ++;
    }

  tr_event_->Fill();
  return Fun4AllReturnCodes::EVENT_OK;
}


int JetAnalyzerStudy::ResetEvent(PHCompositeNode *topNode)
{
  jet_num_ = 0;
  jet_id_ = 0;
  jet_pt_ = 0.0;
  jet_num_ = 0;
  jet_eta_ = -9999.;
  jet_phi_ = -9999.;
  jet_e_ = -9999.;
  jet_nconst_ = -1;

  vertex_z_ = -9999.9; 

  trigger_vector_.clear();
  live_vector_.clear();
  scaled_vector_.clear();
  gtm_busy_vector_.clear();
  
  return Fun4AllReturnCodes::EVENT_OK;
}

int JetAnalyzerStudy::EndRun(const int runnumber)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

int JetAnalyzerStudy::End(PHCompositeNode *topNode)
{

  tf_->WriteTObject( tr_jet_, tr_jet_->GetName() );
  tf_->WriteTObject( tr_event_, tr_event_->GetName() );
  tf_->WriteTObject( hist_jet_pt_, hist_jet_pt_->GetName() );
  
  tf_->Close();
  
  return Fun4AllReturnCodes::EVENT_OK;
}

int JetAnalyzerStudy::Reset(PHCompositeNode *topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

void JetAnalyzerStudy::Print(const std::string &what) const
{
  std::cout << "Output: " << output_ << std::endl;
}

// General function to get trigger information from uint64_t
std::vector < int > JetAnalyzerStudy::GetFlagVector( uint64_t arg )
{
  std::vector < int > flag_vector( 64 );
  for( int i=0; i<64; i++ )
    {
      int flag = (arg >> i) & 1 ; //1: true, 0: false
      flag_vector[i] = flag;
    }

  return flag_vector;
}

// You can get GL1 information (=trigger info).
int JetAnalyzerStudy::ProcessGl1( PHCompositeNode* topNode )
{

  Gl1Packet *gl1_packet = findNode::getClass<Gl1Packet>(topNode, gl1_packet_name_ );
  if( gl1_packet == nullptr )
    {
      std::cerr << "GL1Packet is not found." << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }

  // event_ = gl1_packet->getEvtSequence();
  // bco_ = gl1_packet->getBCO();

  ////////////////////////
  // Gl1Packetv2
  ////////////////////////
  event_id_                             = gl1_packet->getEvtSequence();
  //  uint64_t trigger_input            = gl1_packet->getTriggerInput();

  uint64_t live_vector_value            = gl1_packet->getLiveVector();
  uint64_t trigger_vector_value         = gl1_packet->getTriggerVector();
  uint64_t scaled_vector_value          = gl1_packet->getScaledVector();
  uint64_t gtm_busy_vector_value        = gl1_packet->getGTMBusyVector();

  trigger_vector_                       = this->GetFlagVector( trigger_vector_value );
  live_vector_                          = this->GetFlagVector( live_vector_value );
  scaled_vector_                        = this->GetFlagVector( scaled_vector_value );
  gtm_busy_vector_                      = this->GetFlagVector( gtm_busy_vector_value );

  return 0;
}

// You can get MBD vertex information
int JetAnalyzerStudy::MbdVertex(PHCompositeNode *topNode )
{
  /// Get the global vertex to determine the appropriate pseudorapidity of the clusters
  GlobalVertexMap *vertexmap = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");
  //vertexmap->identify();

  if (!vertexmap)
    {
      std::cerr << "GlobalVertexMap node is missing. Please turn on the do_global flag in the main macro in order to reconstruct the global vertex." << std::endl;
      assert(vertexmap);  // force quit
      return -1;
    }
  else
    {
      if( this->Verbosity() > 2 )
        {
          vertexmap->identify();
        }
    }

  //GlobalVertex *vtx = nullptr;

  // loop over all contents (can be MBD, truth, SVTX, etc.)
  for(auto iter = vertexmap->begin(); iter!= vertexmap->end(); ++iter)
    {
      // unsigned int vertex_index = iter->first;
      GlobalVertex* vertex = iter->second;

      if( this->Verbosity() > 2 )
        {
          std::cout << "undefined " << std::setw(5) << vertex->count_vtxs( GlobalVertex::UNDEFINED ) << " "
                    << "truth " << std::setw(5) << vertex->count_vtxs( GlobalVertex::TRUTH ) << " "
                    << "smeared "<< std::setw(5) << vertex->count_vtxs( GlobalVertex::SMEARED ) << " "
                    << "mbd " << std::setw(5) << vertex->count_vtxs( GlobalVertex::MBD ) << " "
                    << "svtx " << std::setw(5) << vertex->count_vtxs( GlobalVertex::SVTX ) << " "
                    << "svtx_mbd " << std::setw(5) << vertex->count_vtxs( GlobalVertex::SVTX_MBD ) << " "
                    << std::endl;
        }

      // MBD vertex should be only one
      if( vertex->count_vtxs( GlobalVertex::MBD ) != 1 )
        {
          continue;
        }

      for( GlobalVertex::ConstVertexIter iter = vertex->begin_vertexes(); iter != vertex->end_vertexes(); ++iter)
        {
          if( iter->first != GlobalVertex::MBD )
            continue;

          const Vertex* ver_mbd = (iter->second)[ 0 ]; // the number of associated vertices should be one in the case of MBD

          //vertex_ = ROOT::Math::XYZTVector( ver_mbd->get_x(), ver_mbd->get_y(), ver_mbd->get_z(), ver_mbd->get_t() );
          vertex_z_ = ver_mbd->get_z();
          // vertex_chi2_ = ver_mbd->get_chisq();
          // vertex_ndf_ = ver_mbd->get_ndof(); // the number of degree of freedom
          // vertex_status_ = ver_mbd->isValid();

        } // End of for( GlobalVertex::ConstVertexIter iter = vertex->begin_vertexes(); iter != vertex->end_vertexes(); ++iter)
    } // end of for(auto iter = vertexmap->begin(); iter!= vertexmap->end(); ++iter)

  return 0;
}
