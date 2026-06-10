#include "Runner.hh"

#include <G4MTRunManager.hh>
#include <G4RunManagerFactory.hh>
#include <CeleritasG4.hh>

#include "DetectorConstruction.hh"
#include "PhysicsList.hh"
#include "ActionInitialization.hh"

celeritas::SetupOptions celeritas_options(OpticalHitRecorder* hit_recorder)
{
    celeritas::SetupOptions so;
    so.max_num_tracks = 1024;
    so.initializer_capacity = 1024;
    so.secondary_stack_factor = 2.0;
    so.output_file = "";
    so.make_along_step = celeritas::UniformAlongStepFactory();

    so.optical = [hit_recorder] {
        celeritas::OpticalSetupOptions opts;
        opts.capacity = celeritas::inp::OpticalStateCapacity::from_default(false);
        opts.generator = celeritas::inp::OpticalOffloadGenerator{};
        opts.detectors.callback = [hit_recorder] (celeritas::Span<celeritas::optical::DetectorHit const> hits) { (*hit_recorder)(hits); };
        return opts;
    }();

    so.offload_particles = std::vector<G4ParticleDefinition*>{};
    return so;
}

Runner::Runner(inp::Config config)
    : config_(std::move(config))
{
    run_manager_.reset(G4RunManagerFactory::CreateRunManager(config.num_threads == 1 ? G4RunManagerType::Serial : G4RunManagerType::MT));

    if (auto* rm_mt = (G4MTRunManager*) run_manager_.get())
    {
        rm_mt->SetNumberOfThreads(config.num_threads);
    }

    hit_recorder_ = std::make_unique<OpticalHitRecorder>();

    run_manager_->SetUserInitialization(new DetectorConstruction(config_, hit_recorder_.get()));
    run_manager_->SetUserInitialization(new PhysicsList(config_.detector.allowed_volumes));
    run_manager_->SetUserInitialization(new ActionInitialization(config_));

    celeritas::UserActionIntegration::Instance().SetOptions(celeritas_options(hit_recorder_.get()));

    run_manager_->Initialize();
}

void Runner::operator()()
{
    for (unsigned int i = 0; i < config_.primary.energies.size(); i++)
    {
        G4cout << "Run " << i << " energy " << config_.primary.energies[i] << G4endl;
        run_manager_->BeamOn(config_.num_events);
    }
}
