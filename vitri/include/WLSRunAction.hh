/// \file optical/wls/include/WLSRunAction.hh
/// \brief Definition of the WLSRunAction class
//

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#ifndef WLSRunAction_h
#define WLSRunAction_h 1

#include "G4UserRunAction.hh"

class WLSRun;
class G4Run;

class WLSRunAction : public G4UserRunAction
{
  public:
    WLSRunAction();
    ~WLSRunAction() override = default;

    void BeginOfRunAction(const G4Run*) override;
    void EndOfRunAction(const G4Run*) override;
    G4Run* GenerateRun() override;

  private:
    WLSRun* fRun = nullptr;
};

#endif
