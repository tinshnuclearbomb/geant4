/// \file optical/wls/include/WLSRun.hh
/// \brief Definition of the WLSRun class
//

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#ifndef WLSRun_h
#define WLSRun_h 1

#include "G4Run.hh"

class WLSRun : public G4Run
{
  public:
    WLSRun() = default;
    ~WLSRun() override = default;

    void AddTIR(G4int n)
    {
      G4double nd(n);
      fNTIR += nd;
      fNTIR2 += nd * nd;
    };
    void AddExiting(G4int n)
    {
      G4double nd(n);
      fNExiting += nd;
      fNExiting2 += nd * nd;
    };
    void AddEscapedEnd(G4int n)
    {
      G4double nd(n);
      fEscapedEnd += nd;
      fEscapedEnd2 += nd * nd;
    };
    void AddEscapedMid(G4int n)
    {
      G4double nd(n);
      fEscapedMid += nd;
      fEscapedMid2 += nd * nd;
    };
    void AddBounce(G4int n)
    {
      G4double nd(n);
      fBounce += nd;
      fBounce2 += nd * nd;
    };
    void AddWLSBounce(G4int n)
    {
      G4double nd(n);
      fWLSBounce += nd;
      fWLSBounce2 += nd * nd;
    };
    void AddClad1Bounce(G4int n)
    {
      G4double nd(n);
      fClad1Bounce += nd;
      fClad1Bounce2 += nd * nd;
    };
    void AddClad2Bounce(G4int n)
    {
      G4double nd(n);
      fClad2Bounce += nd;
      fClad2Bounce2 += nd * nd;
    };
    void AddReflected(G4int n)
    {
      G4double nd(n);
      fReflected += nd;
      fReflected2 += nd * nd;
    };
    void AddEscaped(G4int n)
    {
      G4double nd(n);
      fEscaped += nd;
      fEscaped2 += nd * nd;
    };
    void AddMirror(G4int n)
    {
      G4double nd(n);
      fMirror += nd;
      fMirror2 += nd * nd;
    };
    void AddDetectorHits(G4int n)
    {
      G4double nd(n);
      fDetectorHits += nd;
      fDetectorHits2 += nd * nd;
    };

    void EndOfRun();
    void Merge(const G4Run*) override;

  private:
    G4double fNTIR = 0.;
    G4double fNTIR2 = 0.;
    G4double fNExiting = 0.;
    G4double fNExiting2 = 0.;
    G4double fEscapedEnd = 0.;
    G4double fEscapedEnd2 = 0.;
    G4double fEscapedMid = 0.;
    G4double fEscapedMid2 = 0.;
    G4double fBounce = 0.;
    G4double fBounce2 = 0.;
    G4double fWLSBounce = 0.;
    G4double fWLSBounce2 = 0.;
    G4double fClad1Bounce = 0.;
    G4double fClad1Bounce2 = 0.;
    G4double fClad2Bounce = 0.;
    G4double fClad2Bounce2 = 0.;
    G4double fReflected = 0.;
    G4double fReflected2 = 0.;
    G4double fEscaped = 0.;
    G4double fEscaped2 = 0.;
    G4double fMirror = 0.;
    G4double fMirror2 = 0.;
    G4double fDetectorHits = 0.;
    G4double fDetectorHits2 = 0.;
};

#endif
