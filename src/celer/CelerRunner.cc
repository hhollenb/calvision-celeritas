#include "CelerRunner.hh"

#include "celeritas/inp/StandaloneInput.hh"

#include <iostream>

class HitCallback
{
  public:
    HitCallback(OpticalHitRecorder* record_hit, DetectorMapping* detector_mapping)
        : record_hit_(record_hit)
        , detector_mapping_(detector_mapping)
    {}

    void operator()(celeritas::Span<celeritas::optical::DetectorHit const> hits)
    {
        OpticalHit result;
        for (auto const& hit : hits)
        {
            result.energy = celeritas::value_as<celeritas::units::MevEnergy>(hit.energy) * 1e6;
            result.time = hit.time / celeritas::units::nanosecond.value();
            result.detector_id = (*detector_mapping_)(hit.volume_unique_instance);
            result.gen_type = hit.gen_type;
            (*record_hit_)(result);
        }
    }

  private:
    OpticalHitRecorder* record_hit_;
    DetectorMapping* detector_mapping_;
};


CelerRunner::CelerRunner()
    : edep_reader_(std::make_unique<EnergyDepositReader>())
{
    detector_mapping_ = DetectorMapping::full_matrix();
    hit_recorder_ = std::make_unique<OpticalHitRecorder>("celercpu");

    celeritas::inp::OpticalStandaloneInput inp;
    inp.system.device = std::nullopt;

    inp.problem.model.geometry = "dual_readout.gdml";
    inp.problem.generator = celeritas::inp::OpticalOffloadGenerator{};
    inp.problem.capacity = celeritas::inp::OpticalStateCapacity::from_default(false);
    inp.problem.detectors.callback = HitCallback{hit_recorder_.get(), detector_mapping_.get()};
    inp.problem.num_streams = 1;


    inp.geant_setup.cherenkov.emplace();
    inp.geant_setup.scintillation.emplace();
    inp.geant_setup.boundary.emplace();
    inp.geant_setup.absorption = true;
    inp.geant_setup.rayleigh_scattering = true;
    inp.geant_setup.mie_scattering = false;
    inp.geant_setup.wavelength_shifting = {};
    inp.geant_setup.wavelength_shifting2 = {};

    inp.detectors = {"PhotonDetector"};

    runner_ = std::make_unique<celeritas::optical::Runner>(std::move(inp));

    detector_mapping_->construct();
    hit_recorder_->set_num_detectors(detector_mapping_->num_detectors());
}

void CelerRunner::operator()()
{
    unsigned int num_events = 1;
    for (unsigned int i = 0; i < num_events; i++)
    {
        std::cout << "Starting event " << i << "\n";
        this->run_event(i);
    }
}

void CelerRunner::run_event(unsigned int event)
{
    std::cout << "\tStarting hit recorder for event " << event << "\n";
    hit_recorder_->start_event(event);
    std::cout << "\tLoading edeps...\n";
    auto const hits = (*edep_reader_)("edep_" + std::to_string(event) + ".root");
    std::cout << "\tLoaded " << hits.size() << " edeps\n";
    runner_->insert(celeritas::make_span(std::as_const(hits)));
    std::cout << "\tRunning...\n";
    (*runner_)();
    std::cout << "\tEnding event...\n";
    hit_recorder_->end_event();
    std::cout << "\tDone.\n";
}
