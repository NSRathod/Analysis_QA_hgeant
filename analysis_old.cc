#include "analysis.h"
#include "hntuple.h"
#include "HFilter.h"
#include <TH1F.h>
#include <TComplex.h>


using namespace std;


// ------------------------------------------------------------------------------------------------------------------
   const double D2R = 1.74532925199432955e-02;
   const double R2D = 57.2957795130823229;
// ------------------------------------------------------------------------------------------------------------------
   double openingangle(TLorentzVector a, TLorentzVector b)
   {
         return TMath::ACos( (a.Px()*b.Px() + a.Py()*b.Py() +  a.Pz()*b.Pz() ) / ( a.Vect().Mag() * b.Vect().Mag() ) );
   }
// ==================================================================================================================
// ------------------------------------------------------------------------------------------------------------------
   void normalize(TH1* hist)
   {
      for (Int_t j=1; j<hist->GetNbinsX()+1; ++j)
      {
         hist->SetBinContent( j, hist->GetBinContent(j) / hist->GetBinWidth(j) );
//         hist->SetBinError( j, TMath::Sqrt( hist->GetBinContent(j) ) );
         hist->SetBinError( j, hist->GetBinError(j) / hist->GetBinWidth(j) );
      }
   }
// ==================================================================================================================
// ------------------------------------------------------------------------------------------------------------------
Float_t transformPhi(Float_t Phi)
{
   Float_t dPhi;

   if( (dPhi = TMath::RadToDeg() * Phi) < 0.0 )
      dPhi += 360.0;

   return dPhi;
}
// ==================================================================================================================
// ------------------------------------------------------------------------------------------------------------------
Bool_t isGeantTrackInAcceptance(HGeantKine *pG, HIterator *pitMdcGeant, HIterator *pitTofGeant, HIterator *pitRPCGeant, HIterator *pitShowerGeant);
Bool_t isGeantTrackInFWall(HGeantKine *pG, HIterator *pitFWGeant);
// ------------------------------------------------------------------------------------------------------------------


// ------------------------------------------------------------------------------------------------------------------
//Int_t analysis(TString infile = "")
Int_t analysis()
{ 
 Hades* myHades = new Hades();
 gHades -> setQuietMode(2);
 
 // ****  INPUT ****

 TString infile = "testout1";
 TString inputDir  = "/lustre/nyx/hades/user/iciepal/pNb/geant_input/";
 TString outputDir = "/lustre/nyx/hades/user/iciepal/pNb/geant_input/";


 
 TString infile1 = infile+".root";
 TString outfile1 = infile+"_1.root";
 
 HRootSource *source = new HRootSource;
 source -> setDirectory((char*)inputDir.Data());
 source -> addFile((char*)infile1.Data());
 gHades -> setDataSource(source);
 if(!gHades->init())
   {
     cout << "gHades->init() ERROR " << endl;
     exit(1);
   }
 
// Getting the categories
 // geant
 HCategory* pGeantCat = (HCategory*)gHades->getCurrentEvent()->getCategory(catGeantKine);
 HIterator* pitGeant = (HIterator *)pGeantCat->MakeIterator();
 // mdc 
 HCategory* pMdcGeantCat = (HCategory*)gHades->getCurrentEvent()->getCategory(catMdcGeantRaw);
 HIterator* pitMdcGeant = (HIterator *)pMdcGeantCat->MakeIterator();
 // tof
 HCategory* pTofGeantCat = (HCategory*)gHades->getCurrentEvent()->getCategory(catTofGeantRaw);
 HIterator* pitTofGeant = (HIterator *)pTofGeantCat->MakeIterator();
 // shower
 HCategory* pShowerGeantCat = (HCategory*)gHades->getCurrentEvent()->getCategory(catShowerGeantRaw);
 HIterator* pitShowerGeant = (HIterator *)pShowerGeantCat->MakeIterator();
 // rpc
 HCategory* pRPCGeantCat = (HCategory*)gHades->getCurrentEvent()->getCategory(catRpcGeantRaw);
 HIterator* pitRPCGeant = (HIterator *)pRPCGeantCat->MakeIterator();
 // wall
 HCategory* pFWallGeantCat = (HCategory*)gHades->getCurrentEvent()->getCategory(catWallGeantRaw);
 HIterator* pitFWGeant = (HIterator *)pFWallGeantCat->MakeIterator();

 
 
 TFile ff(outputDir+outfile1,"recreate");


 //---------------------------------------
 double p_momentum = 3500.0; 

 //---------------------------------------


 double p_energy = sqrt( p_momentum*p_momentum + 938.27231*938.27231);

   TLorentzVector *proj = new TLorentzVector(0,0, p_momentum, p_energy); // PION BEAM momentum as above
   TLorentzVector *targ = new TLorentzVector(0,0,0,85590.7); // Nb-92
/*******************************************************************************************************/
   TLorentzVector *beam = new TLorentzVector(0,0,0,0);
   *beam = *proj + *targ;

 TLorentzVector*  p = new TLorentzVector(0,0,0,0);
 TLorentzVector*  p_LAB = new TLorentzVector(0,0,0,0);

/*******************************************************************************************************/

 TH2F* p_mom_theta_4pi = new TH2F("p_mom_theta_4pi","p_mom_theta_4pi",70,0,700,90,0,180); p_mom_theta_4pi->Sumw2();
 TH1F *PiM_theta_cm   = new TH1F("PiM_theta_cm" ,"PiM theta cm",53,11.5,170.5);

/*******************************************************************************************************/
 HGeantKine  *pKine = 0;
 TVector3    vec1, vec2, vec3, vec4;

 bool pAcc = false, pimAcc = false;


 
 Int_t  nentries = 100000;
 for (Int_t i = 0; i<nentries; i++)
   {//event loop
     if(!gHades->eventLoop(1)) 
     {
        if(i<100000) cout<<"TOO SMALL FILE "<<i<<endl;
        break;
     }
    


     if(i%10000==0) cout<<" --- Current event: "<<i<<endl;

     pAcc = false;
     pimAcc = false;

     pitGeant->Reset();
     while((pKine = (HGeantKine*)pitGeant->Next()) != 0)
	 {

	       pKine->getParticle( aTrackLepton, aID );
	       pKine->getCreator ( aParentID, aMedLepton, aMechLepton );
	       pKine->getGenerator( genInfo, genWeight );
	       pKine->getMomentum( xMom, yMom, zMom );
           //cout << " parent = " << aParentID << " id = " << aID << " geninf = " << genInfo << endl;
       /************************************************************ PID ***************************/
	   if(aParentID == 0 && aID == 14 ) // p
       {
          aID1 = aID;
          xMom1 = xMom;
          yMom1 = yMom;
          zMom1 = zMom;
          genWeight1 = genWeight;
          if ( isGeantTrackInAcceptance(pKine, pitMdcGeant, pitTofGeant, pitRPCGeant, pitShowerGeant) ) pAcc = true;
       }
	   if(aParentID == 0 && aID == 9 ) // pi-
       {
          aID2 = aID;
          xMom2 = xMom;
          yMom2 = yMom;
          zMom2 = zMom;
          genWeight2 = genWeight;
          if ( isGeantTrackInAcceptance(pKine, pitMdcGeant, pitTofGeant, pitRPCGeant, pitShowerGeant) ) pimAcc = true;
       }
    } // GeantKine loop

    vec1.SetXYZ( xMom1, yMom1, zMom1 );
    vec2.SetXYZ( xMom2, yMom2, zMom2 );
    p_LAB->SetVectM(vec1, 938.27231); // p
    pim_LAB->SetVectM(vec2, 139.56995); // pi-

    double p_p = p_LAB->P();
    double p_theta = p_LAB->Theta()*TMath::RadToDeg();
    double p_phi = transformPhi(p_LAB->Phi());
    double pim_p = pim_LAB->P();
    double pim_theta = pim_LAB->Theta()*TMath::RadToDeg();
    double pim_phi = transformPhi(pim_LAB->Phi());

    p_mom_theta_4pi->Fill( p_p, p_theta );
    p_mom_theta_phi_4pi->Fill( p_p, p_theta, p_phi );
    if (pAcc) {
        p_mom_theta_acc->Fill( p_p, p_theta );
        p_mom_theta_phi_acc->Fill( p_p, p_theta, p_phi );
    }

    pim_mom_theta_4pi->Fill( pim_p, pim_theta );
    pim_mom_theta_phi_4pi->Fill( pim_p, pim_theta, pim_phi );
    if (pimAcc) {
        pim_mom_theta_acc->Fill( pim_p, pim_theta );
        pim_mom_theta_phi_acc->Fill( pim_p, pim_theta, pim_phi );
    }

    //cout << " p " << p_p <<","<<p_theta<<","<<p_phi<< endl;
    //cout << " pim " << pim_p <<","<<pim_theta<<","<<pim_phi<< endl;

       *p = *p_LAB;
       *pim = *pim_LAB;

       *beam = *p_LAB + *pim_LAB;

      pim->Boost(0.0, 0.0, -(*beam).Beta() );
      p->Boost(0.0, 0.0, -(*beam).Beta() );

      PiM_theta_cm->Fill( R2D * pim->Theta(), genWeight1 );

       // *********************

       // * * * * * * correction part * * * * * * 

         //int pim_sector = static_cast<int> ( transformPhi( pim->Phi() ) / 60 );
         //int p_sector = static_cast<int> ( transformPhi( p->Phi() ) / 60 );
         //int pim_system = pim_theta < 45. ? 0 : 1;
         //int p_system = p_theta < 45. ? 0 : 1;


   } // event loop
     
 ff.cd();
 //nt->Write();

 p_mom_theta_4pi->Write();
 p_mom_theta_acc->Write();
 p_mom_theta_phi_4pi->Write();
 p_mom_theta_phi_acc->Write();
 pim_mom_theta_4pi->Write();
 pim_mom_theta_acc->Write();
 pim_mom_theta_phi_4pi->Write();
 pim_mom_theta_phi_acc->Write();

 PiM_theta_cm->Write();

 ff.Close();

 delete myHades;
 
 return 1;
}
// ==================================================================================================================





// ------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------------
int main(int argc, char **argv)
{
  analysis(TString(argv[1]));
  return 0;
}
// ==================================================================================================================
// ==================================================================================================================
// ==================================================================================================================



// ------------------------------------------------------------------------------------------------------------------
Bool_t isGeantTrackInAcceptance(HGeantKine *pG, HIterator *pitMdcGeant, HIterator *pitTofGeant, HIterator *pitRPCGeant, HIterator *pitShowerGeant)
{
     Int_t lTrack, lId;
     pG->getParticle(lTrack,lId);

     Int_t nStatMDC1 = 0, nStatMDC2 = 0, nStatMDC3 = 0,  nStatMDC4 = 0, nStatTof = 0, nStatRPC = 0, nStatShower = 0;

     HGeantMdc *pMdc;
     pitMdcGeant->Reset();
     while((pMdc = (HGeantMdc*) pitMdcGeant->Next()) != 0){
             if (pMdc->getTrack() == lTrack)
             {
                switch (((Int_t)pMdc->getModule()))
                {
                    case 0: nStatMDC1 = 1; break;
                    case 1: nStatMDC2 = 1; break;
                    case 2: nStatMDC3 = 1; break;
                    case 3: nStatMDC4 = 1; break;
                    default: cerr << "WRONG MDC module number!" << endl;
                }
             }
     }

     HGeantTof *pTof;
     pitTofGeant->Reset();
     while((pTof = (HGeantTof*) pitTofGeant->Next()) != 0){

            if (pTof->getTrack() == lTrack)
            {
               nStatTof = 1;
            }
     }

     HGeantRpc *pRPC;
     pitRPCGeant->Reset();
     while((pRPC = (HGeantRpc*) pitRPCGeant->Next()) != 0){

            if (pRPC->getTrack() == lTrack)
            {
               nStatRPC = 1;
            }

     }

     HGeantShower *pShower;
     pitShowerGeant->Reset();
     while((pShower = (HGeantShower*) pitShowerGeant->Next()) != 0){

            if (pShower->getTrack() == lTrack)
            {
               nStatShower = 1;
            }

     }

   //cout << " MDC: " << nStatMDC1 <<","<<nStatMDC2<<","<<nStatMDC3<<","<<nStatMDC4<<" TOF RPC SHW "<<nStatTof<<","<<nStatRPC<<","<<nStatShower<<endl;
   
   if (nStatMDC1 && nStatMDC2 && nStatMDC3 &&  nStatMDC4 &&  (nStatTof || nStatRPC || nStatShower)) return kTRUE;

   // below is a kind of debug in case of wrong data
   //if (pG->getNMdcHits(0) && pG->getNMdcHits(1) && pG->getNMdcHits(2) && (pG->getNMdcHits(0)>7 || pG->getNMdcHits(1)>7 || pG->getNMdcHits(2)>7))
   // {
   //     cout << " momentum = " << pG->getTotalMomentum();
   //     cout << " nStatMDC1: " << pG->getNMdcHits(0);
   //     cout << " nStatMDC2: "<< pG->getNMdcHits(1);
   //     cout << " nStatMDC3: "<< pG->getNMdcHits(2);
   //     cout << " nStatMDC4: "<< pG->getNMdcHits(3) <<endl;
   // }

   return kFALSE;
}
// ==================================================================================================================

//-----------------------------------------------------------------------------------------------------------------
Bool_t isGeantTrackInFWall(HGeantKine *pG, HIterator *pitFWGeant)
{
     Int_t lTrack, lId;
     pG->getParticle(lTrack,lId);

     Int_t nStatFW = 0;
     HGeantWall *pFW;
     pitFWGeant->Reset();
     while((pFW = (HGeantWall*) pitFWGeant->Next()) != 0){

            if (pFW->getTrack() == lTrack)
            {
               nStatFW = 1;
            }

     }

   if (nStatFW) return kTRUE;

   return kFALSE;
}
// ==================================================================================================================


