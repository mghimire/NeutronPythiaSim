#include <unistd.h>
#include <cstdlib>
#include <sstream>
#include <string>
#include <vector>
#include <random>

#include "Pythia8/Pythia.h"

#include "TFile.h"
#include "TMath.h"
#include "TROOT.h"
#include "TTree.h"
#include "TVectorD.h"

using namespace Pythia8;

// struct of information from each event that will be filled in ROOT tree
typedef struct {
  Int_t id;          // ID of neutron (2112)
  Double_t E;	     // energy of neutron
  Double_t pT;       // pT of neutron
  Double_t eta;      // eta (pseudorapidity) of neutron
  Double_t pTHat;    // event's pTHat
  Double_t weight;   // event's weight (normally 1)
  UInt_t event_num;  // event number neutron came from
} n_event;

int main(int argc, char **argv) {
  // set default options
  // number of events to generate
  int nEvent = 100;
  // new neutron mass in GeV
  double n_mass = 0.939565;
  // jet pT (pTHat) cut in GeV
  double pTcut = 4.0;
  // bool to check if pTHat cut is provided manually
  bool pTInput = false;
  // variable for manual pTHat cut
  double manualpTcut;
  // name of output root file with events
  TString output_file = "out.root";
  // output found particle info
  bool verbose = false;
  //type of events to make
  int myprocess=0;

  // process commandline arguments
  int c;
  while ((c = getopt(argc, argv, "vf:n:m:p:t:")) != -1) switch (c) {
      case 'v':  // verbose, output mCP info
        verbose = true;
        break;
      case 'f':  // output root file name
        output_file = optarg;
        break;
      case 'n':  // number of events to generate
        nEvent = std::stoi(optarg);
        break;
      case 'm':  // mCP mass in GeV
        n_mass = std::stod(optarg);
        break;
      case 'p':  // jet pT (pTHat) cut in GeV
        manualpTcut = std::stod(optarg);
		pTInput = true;
        break;
      case 't':  // type of process to make
        myprocess = std::stod(optarg);
        break;
      case '?':
        cout << "Error: Invalid option" << endl;
        return EXIT_FAILURE;
  }
    
  // Check to update pTcut
  if (pTInput){
    pTcut = manualpTcut;
  }

  // Generator
  Pythia pythia;

  // Use random seed each run
  std::random_device r;
  std::mt19937 mt(r());
  std::uniform_int_distribution<int> uniform_dist(1, 900000000);
  int ran_seed = uniform_dist(mt);
  std::ostringstream strsRanSeed;
  strsRanSeed << ran_seed;
  pythia.readString("Random:setSeed = on");
  pythia.readString("Random:seed = " + strsRanSeed.str());
  
  if (myprocess==0){    
	pythia.readString("HardQCD:all = on"); // Turn on hard QCD processes
	
	// Apply jet pT cut
	std::ostringstream strspT;
	strspT << pTcut;
	pythia.readString("PhaseSpace:pTHatMin = " + strspT.str());
  }
  else if (myprocess==1){ 
       pythia.readString("Onia:all = on"); // Turn on all *onia processes
       
       // Apply a pT cut
       double oniaptcut = 2.0; // was 2 GeV for Upsilon and 6.5 GeV for J/psi in original paper (as well as |eta|<2.5)
       std::ostringstream strspT;
	   strspT << oniaptcut;
	   pythia.readString("PhaseSpace:pTHatMin = " + strspT.str());
  }
  else if (myprocess==2){
       pythia.readString("WeakSingleBoson:ffbar2gmZ= on"); // Turn on gamma* and Z
       
       // Apply mass cut
       double mcut = 2.0; // always above 2 GeV, to match original mq paper
       if (n_mass*2. -1 > mcut){
          mcut = n_mass*2. -1;
          std::ostringstream strsm;
          strsm << mcut;
          pythia.readString("PhaseSpace:mHatMin = " + strsm.str());
       }
  }
  else {
         cout << "invalid process: " << myprocess<<endl;
         return -7;
  }
  
  //Turn off hadronization
  //pythia.readString("HadronLevel:Hadronize = off");

  // Initialization, pp beam @ 13 TeV
  pythia.readString("Beams:idA = 2212");
  pythia.readString("Beams:idB = 2212");
  //pythia.readString("Beams:eCM = 13000");//13 TeV
  pythia.readString("Beams:eCM = 13600");//14 TeV

  // reweight events by power in pT to get more favorable spectrum
  // setting to off will put event weights back to 1
  /*pythia.readString("PhaseSpace:bias2Selection = on");
  pythia.readString("PhaseSpace:bias2SelectionPow = 1.1");*/

  // change neutron mass in Pythia
  // std::ostringstream strsmCP;
  // strsn << n_mass;
  // pythia.readString("2112:m0 = " + strsn.str());

  // initialize pythia
  pythia.init();
  
  // enable this to see output of all particle data
  // pythia.particleData.listAll();
  
  // cout<< "Z BR to +-13 now "<< pythia.particleData.particleDataEntryPtr(23)->channel(7).bRatio()<<endl;

  // (re)make output file, event tree
  TFile f(output_file, "recreate");
  TTree t1("EventTree", "events tree");

  // variable to store events for tree filling
  n_event cpevent;

  // set up TTree branches to read in
  t1.Branch("id", &cpevent.id, "id/I");
  t1.Branch("E", &cpevent.E, "E/D");
  t1.Branch("pT", &cpevent.pT, "pT/D");
  t1.Branch("eta", &cpevent.eta, "eta/D");
  t1.Branch("pTHat", &cpevent.pTHat, "pTHat/D");
  t1.Branch("weight", &cpevent.weight, "weight/D");
  t1.Branch("event_num", &cpevent.event_num, "event_num/i");


  int neutron_n = 0;
  // loop to generate events
  for (int iEvent = 0; iEvent < nEvent; ++iEvent) {
    // generate event, skip if error
    if (!pythia.next()) continue;

    // list of IDs of mCP that came in pairs
    std::vector<int> neutrons;

    // loop through particles checking daughters for neutrons
    for (int i = 0; i < pythia.event.size(); ++i) {
        if (pythia.event[i].isFinal()) {
          if (pythia.event[i].id() == 2112) {
             if (pythia.event[i].eta() > 0 && pythia.event[i].eta() < 5) neutron_n++;
	     if (pythia.event[i].eta() > 0 && pythia.event[i].eta() < 0.2) neutrons.push_back(i);
          }
        }
    }


    // set TTree variable with each mCP's information then fill it
    for (int n : neutrons) {
      cpevent.id = pythia.event[n].id();
      cpevent.E = pythia.event[n].e();
      cpevent.pT = pythia.event[n].pT();
      cpevent.eta = pythia.event[n].eta();
      cpevent.pTHat = pythia.info.pTHat();
      cpevent.weight = pythia.info.weight();
      cpevent.event_num = iEvent;
      t1.Fill();

      // output found particle information
      if (verbose) {
        cout << pythia.event[n].name() << ":" << endl;
        cout << "pT = " << pythia.event[n].pT() << endl;
      }
    }

    // visual divider between events for output
    if (verbose && neutrons.size() > 0) cout << "---------" << endl;
  }

  // output pythia stats
  pythia.stat();

  cout << "Total number of neutrons generated: " << neutron_n << endl;

  // set TTree weight to normalize to cross section and per event
//  Double_t sigma = pythia.info.sigmaGen();       // total cross section
//  Double_t sigmaerr = pythia.info.sigmaErr();    // cross section error
//  Double_t weightsum = pythia.info.weightSum();  // sum of weights (# events)

  // calculate tree weight and error
  // Double_t tree_weight = sigma / weightsum;
  // Double_t tree_weight_error = sigmaerr / weightsum;

  // cout << "sigma is " << sigma << endl;
  // cout << "weightsum is " << weightsum << endl;
  // cout << "tree weight is " << tree_weight << endl;

  // store weight error and mass in TTree
  // TVectorD sig_err(2);
  // sig_err[0] = tree_weight_error;
  // sig_err[1] = n_mass;
  // t1.GetUserInfo()->Add(&sig_err);

  // store weight as tree weight
  // t1.SetWeight(tree_weight);

  // write the tree to disk
  t1.Write();

  // output number of events
  int num_neutrons = t1.GetEntries();
  cout << "Recorded " << num_neutrons << " events to " << output_file << endl;
  cout << "Number of neutrons recorded/Number of neutrons generated :" << num_neutrons/neutron_n;

  // ROOT may complain about a TList accessing an already deleted object.
  // This may be safely ignored.

  return EXIT_SUCCESS;
}
