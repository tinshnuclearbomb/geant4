/// \file optical/wls/include/WLSMaterials.hh
/// \brief Definition of the WLSMaterials class
//

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#ifndef WLSMaterials_h
#define WLSMaterials_h 1

#include "globals.hh"

class G4Material;
class G4NistManager;

class WLSMaterials
{
  public:
    ~WLSMaterials();

    static WLSMaterials* GetInstance();
    G4Material* GetMaterial(const G4String);

  private:
    WLSMaterials();
    void CreateMaterials();

    static WLSMaterials* fInstance;

    G4NistManager* fNistMan = nullptr;

    G4Material* fAir = nullptr;
    G4Material* fPMMA = nullptr;
    G4Material* fPethylene = nullptr;
    G4Material* fFPethylene = nullptr;
    G4Material* fPolystyrene = nullptr;
    G4Material* fSilicone = nullptr;
    G4Material* fCoating = nullptr;
};

#endif /*WLSMaterials_h*/
