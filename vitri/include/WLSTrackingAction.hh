/// \file optical/wls/include/WLSTrackingAction.hh
/// \brief Definition of the WLSTrackingAction class
//

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#ifndef WLSTrackingAction_h
#define WLSTrackingAction_h 1

#include "G4UserTrackingAction.hh"

class WLSTrackingAction : public G4UserTrackingAction
{
  public:
    WLSTrackingAction() = default;
    ~WLSTrackingAction() override = default;

    void PreUserTrackingAction(const G4Track*) override;
};

#endif
