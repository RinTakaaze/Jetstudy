////////////////////////////////////////////////
// Fun4All headers                            //
////////////////////////////////////////////////
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllDstInputManager.h>

////////////////////////////////////////////////
// something else                             //
////////////////////////////////////////////////
#include <ffamodules/FlagHandler.h>
#include <ffamodules/HeadReco.h>
#include <ffamodules/SyncReco.h>
#include <ffamodules/CDBInterface.h>

#include <phool/PHRandomSeed.h>
#include <phool/recoConsts.h>
#include <TriggerRunInfov1.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libcalotrigger.so)
R__LOAD_LIBRARY(libcalotrigger_io.so)

////////////////////////////////////////////////
// Headers/libraries for my analysis modules  //
////////////////////////////////////////////////
#include <jetanalyzer/JetAnalyzer.h>
R__LOAD_LIBRARY(libJetAnalyzer.so)

////////////////////////////////////////////////
// Some general header macros                 //
////////////////////////////////////////////////
#include <GlobalVariables.C>
#include <G4Setup_sPHENIX.C>
#include <G4_Global.C>
#include <NoBkgdSubJetReco.C>
#include <HIJetReco.C>

#include <Calo_Calib.C>

int Fun4All(
	    int run = 51234,
	    int nEvents = 10,
	    string list_jet = "list/real/dst_jet.txt",
	    string list_calo = "list/real/dst_calo.txt",
	    bool is_debug = true
	    )
{

  if( run < 0 )
    return 0;
  
  //////////////////////////////////////////////////////////////////////////////////////////////////////
  // configuration
  //////////////////////////////////////////////////////////////////////////////////////////////////////
  int verbosity = 3; // Larger number prints more information on your terminal. Default: 0
  
  //////////////////////////////////////////////////////////////////////////////////////////////////////
  // Fun4All general config
  //////////////////////////////////////////////////////////////////////////////////////////////////////
  Fun4AllServer *se = Fun4AllServer::instance();
  //se->Verbosity(0);

  // Read calo DST
  Fun4AllInputManager *in_dst = new Fun4AllDstInputManager("DST_calo");
  in_dst->Verbosity( verbosity );
  in_dst->AddListFile( list_calo );
  se->registerInputManager( in_dst );
  
  // Read jet DST
  Fun4AllInputManager *in_jet = new Fun4AllDstInputManager("DST_jet");
  in_jet->Verbosity( verbosity );
  in_jet->AddListFile( list_jet );
  
  se->registerInputManager( in_jet );
  
  // Flag Handler is always needed to read flags from input (if used)
  // and update our rc flags with them. At the end it saves all flags
  // again on the DST in the Flags node under the RUN node
  FlagHandler *flag = new FlagHandler();
  se->registerSubsystem(flag);
  Enable::CDB = true;
  // global tag

  recoConsts *rc = recoConsts::instance();
  rc->set_StringFlag("CDB_GLOBALTAG","ProdA_2024");  // which should I use?
  //rc->set_StringFlag("CDB_GLOBALTAG",CDB::global_tag);
  // 64 bit timestamp
  //  rc->set_uint64Flag("TIMESTAMP",CDB::timestamp);
  rc->set_uint64Flag("TIMESTAMP", run );
  rc->set_IntFlag("RUNNUMBER", run );

  ///////////////////////////////////////////////////////////////////////////////////
  // Calibration of calorimeters are necessary. Run the function below             //
  ///////////////////////////////////////////////////////////////////////////////////
  Process_Calo_Calib();
  
  /////////////////////////////////////////////////////////////////////////////////////////////
  // HIJetReco: Jets with underlying event suvtraction. Good to use this for AuAu collisions //
  ////////////////////////////////////////////////////////////////////////////////////////////
  // HIJetReco();
  
  ////////////////////////////////////////////////////////////////////////////////////////
  // NoBkgdSubJetReco: No underlying event subtraction, which is good for pp collisions //
  ////////////////////////////////////////////////////////////////////////////////////////
  // NoBkgdSubJetReco(); 

  //////////////////////////
  // analysis module      // 
  //////////////////////////
  JetAnalyzer* analyzer = new JetAnalyzer( "ja_no_sub" );
  se->registerSubsystem( analyzer );

  //se->skip(  ); 
  se->run(nEvents);
  se->End();

  delete se;
  
  gSystem->Exit(0);
  return 0;
}
