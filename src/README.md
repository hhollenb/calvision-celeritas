# Project goals

- Input:
    - edep hits from charged tracks
    - detector geometry
    - sensitive detector mapping
- Output:
    - optical photon hits and histograms


Geant4 simulation:
- Track incident particle through detector
- Hook into Cherenkov and scintillation to record energy depositions
- If optical tracking enabled: also track secondary optical photons and their hits
