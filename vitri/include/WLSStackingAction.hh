/// \file optical/wls/include/WLSStackingAction.hh
/// \brief Definition of the WLSStackingAction class
//

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#ifndef WLSStackingAction_h
#define WLSStackingAction_h 1

#include "G4Types.hh"
#include "G4UserStackingAction.hh"

class WLSStackingAction : public G4UserStackingAction
{
  public:
    WLSStackingAction() = default;
    ~WLSStackingAction() override = default;

    G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track*) override;
    void NewStage() override;
    void PrepareNewEvent() override;

  private:
    G4int fPhotonCounter = 0;
};

#endif
