/// \file optical/wls/include/WLSDetectorMessenger.hh
/// \brief Definition of the WLSDetectorMessenger class
//

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#ifndef WLSDetectorMessenger_h
#define WLSDetectorMessenger_h 1

#include "WLSDetectorConstruction.hh"

#include "G4UImessenger.hh"

class G4UIdirectory;
class G4UIcmdWithABool;
class G4UIcmdWithAString;

class G4UIcmdWithADouble;
class G4UIcmdWithAnInteger;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithoutParameter;
class G4UIcmdWithoutParameter;

class WLSDetectorMessenger : public G4UImessenger
{
  public:
    WLSDetectorMessenger(WLSDetectorConstruction*);
    ~WLSDetectorMessenger() override;

    void SetNewValue(G4UIcommand*, G4String) override;

  private:
    WLSDetectorConstruction* fDetector = nullptr;

    G4UIdirectory* fDetDir = nullptr;

    G4UIcmdWithAString* fSetPhotonDetGeometryCmd = nullptr;
    G4UIcmdWithAnInteger* fSetNumOfCladLayersCmd = nullptr;
    G4UIcmdWithADoubleAndUnit* fSetWLSLengthCmd = nullptr;
    G4UIcmdWithADoubleAndUnit* fSetWLSRadiusCmd = nullptr;
    G4UIcmdWithADoubleAndUnit* fSetClad1RadiusCmd = nullptr;
    G4UIcmdWithADoubleAndUnit* fSetClad2RadiusCmd = nullptr;
    G4UIcmdWithADoubleAndUnit* fSetPhotonDetHalfLengthCmd = nullptr;
    G4UIcmdWithADoubleAndUnit* fSetGapCmd = nullptr;
    G4UIcmdWithADoubleAndUnit* fSetPhotonDetAlignmentCmd = nullptr;
    G4UIcmdWithADouble* fSetXYRatioCmd = nullptr;
    G4UIcmdWithADouble* fSetSurfaceRoughnessCmd = nullptr;
    G4UIcmdWithADouble* fSetMirrorPolishCmd = nullptr;
    G4UIcmdWithADouble* fSetMirrorReflectivityCmd = nullptr;
    G4UIcmdWithADouble* fSetPhotonDetPolishCmd = nullptr;
    G4UIcmdWithADouble* fSetPhotonDetReflectivityCmd = nullptr;
    G4UIcmdWithABool* fSetMirrorCmd = nullptr;
    G4UIcmdWithADoubleAndUnit* fSetBarLengthCmd = nullptr;
    G4UIcmdWithADoubleAndUnit* fSetBarBaseCmd = nullptr;
    G4UIcmdWithADoubleAndUnit* fSetHoleRadiusCmd = nullptr;
    G4UIcmdWithADoubleAndUnit* fSetCoatingThicknessCmd = nullptr;
    G4UIcmdWithADoubleAndUnit* fSetCoatingRadiusCmd = nullptr;
};

#endif
