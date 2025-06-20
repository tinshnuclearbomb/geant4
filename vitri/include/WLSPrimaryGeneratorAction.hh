/// \file optical/wls/include/WLSPrimaryGeneratorAction.hh
/// \brief Definition of the WLSPrimaryGeneratorAction class
//

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#ifndef WLSPrimaryGeneratorAction_h
#define WLSPrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"

class WLSDetectorConstruction;
class WLSPrimaryGeneratorMessenger;

class G4Event;
class G4GeneralParticleSource;
class G4PhysicsTable;

class WLSPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    WLSPrimaryGeneratorAction(WLSDetectorConstruction*);
    ~WLSPrimaryGeneratorAction() override;

    void GeneratePrimaries(G4Event*) override;

    void BuildEmissionSpectrum();
    void SetOptPhotonPolar(G4double);
    void SetDecayTimeConstant(G4double);

    void SetUseSampledEnergy(G4bool v) { fUseSampledEnergy = v; }

  protected:
    G4PhysicsTable* fIntegralTable = nullptr;

  private:
    void SetOptPhotonPolar();
    void SetOptPhotonTime();

    WLSDetectorConstruction* fDetector = nullptr;
    G4GeneralParticleSource* fParticleGun = nullptr;
    WLSPrimaryGeneratorMessenger* fGunMessenger = nullptr;

    static G4bool fFirst;
    G4double fTimeConstant = 0.;
    G4bool fUseSampledEnergy = false;
};

#endif
