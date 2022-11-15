/*
	RESOInputLayer.cpp
	An example of what a user created layer might look like. This is how one would extend the base editor to have more
	functionality, specific to their experiment/setup. In this case, we provide inputs for reaction information so that
	the kinematic shift of the SE-RESO focal plane can be calculated, and weights for tracing particle trajectories are
	produced for use in analysis (as NavVariables).

	A reminder that these layers should not be that intense. The more work that is shoved into the UI, the less responsive
	and more sluggish overall the UI will become. The vast bulk of the analysis work should be left to the PhysicsLayer which has its own
	thread to work upon.

	GWM -- Feb 2022
*/
#include "RESOInputLayer.h"
#include "imgui.h"

namespace Specter {

	RESOInputLayer::RESOInputLayer(const SpectrumManager::Ref& manager) :
		Layer("RESOInputLayer"), 
		m_beam_tke(0.0),
		m_frag_tke(0.0),
		beam_tke("beam_tke"),
		frag_tke("frag_tke"),
		m_rxnEqn("")
	{
		for (int i = 0; i < 2; i++)
		{
			m_targNums[i] = 0;
			m_projNums[i] = 0;
			m_ejectNums[i] = 0;
			m_residNums[i] = 0;
		}

		manager->BindVariable(beam_tke);
		manager->BindVariable(frag_tke);
	}

	RESOInputLayer::~RESOInputLayer() {}

	void RESOInputLayer::OnAttach() {}

	void RESOInputLayer::OnDetach() {}

	void RESOInputLayer::OnUpdate(Timestep& step) {}

	void RESOInputLayer::OnEvent(Event& event) {}

	void RESOInputLayer::OnImGuiRender()
	{
		SPEC_PROFILE_FUNCTION();
		if (ImGui::Begin("RESO Input"))
		{
			//Create widgets for all of our inputs
			ImGui::InputDouble("Beam TKE (MeV)", &m_beam_tke, 0.1, 1.0);
			ImGui::InputDouble("Frag. TKE (MeV)", &m_frag_tke, 0.1, 1.0);
			
			ImGui::InputInt2("Target Z,A", m_targNums);
			ImGui::InputInt2("Beam Z,A", m_projNums);
			ImGui::InputInt2("Ejectile Z,A", m_ejectNums);
			
			if (ImGui::Button("Set"))
			{
				//We dont want to calculate the weights every frame, so
				//we lock that calculation behind a button.
				UpdateValues();
			}
			//Display some info about the internal state
			ImGui::Text("-------Current Settings-------");
			ImGui::Text("Beam. TKE: %f", beam_tke.GetValue());
			ImGui::Text("Frag. TKE: %f", frag_tke.GetValue());
			ImGui::Text("Reaction Equation: ");
			ImGui::SameLine();
			ImGui::Text("%s", m_rxnEqn.c_str());
			// ImGui::Text("X1 Weight: %f", x1_weight.GetValue());
			// ImGui::Text("X2 Weight: %f", x2_weight.GetValue());
		}
		ImGui::End();
	}

	void RESOInputLayer::UpdateValues()
	{
		SPEC_PROFILE_FUNCTION();
		m_rxnEqn = ""; //reset

		//Calculate residual nucleus from reaction
		for (int i = 0; i < 2; i++)
		{
			m_residNums[i] = m_targNums[i] + m_projNums[i] - m_ejectNums[i];
		}
		// if (m_residNums[0] < 0 || m_residNums[1] <= 0)
		// {
		// 	SPEC_ERROR("Invalid residual nucleus at RESOInputLayer::UpdateMasses()! ZR: {0} AR: {1}", m_residNums[0], m_residNums[1]);
		// 	return;
		// }
		// if (m_bfield == 0.0 || m_beamKE == 0.0)
		// {
		// 	SPEC_ERROR("Invaild kinematic settings at RESOInputLayer::UpdateWeights()! BeamKE: {0} Bfield: {1}", m_beamKE, m_bfield);
		// 	return;
		// }

		//Obtain masses from the AMDC table
		double targMass = m_masses.FindMass(m_targNums[0], m_targNums[1]);
		double projMass = m_masses.FindMass(m_projNums[0], m_projNums[1]);
		double ejectMass = m_masses.FindMass(m_ejectNums[0], m_ejectNums[1]);
		double residMass = m_masses.FindMass(m_residNums[0], m_residNums[1]);
		if (targMass == 0.0 || projMass == 0.0 || ejectMass == 0.0 || residMass == 0.0) return;

		std::string temp;
		temp = m_masses.FindSymbol(m_targNums[0], m_targNums[1]);
		m_rxnEqn += temp + "(";
		temp = m_masses.FindSymbol(m_projNums[0], m_projNums[1]);
		m_rxnEqn += temp + ",";
		temp = m_masses.FindSymbol(m_ejectNums[0], m_ejectNums[1]);
		m_rxnEqn += temp + ")";
		temp = m_masses.FindSymbol(m_residNums[0], m_residNums[1]);
		m_rxnEqn += temp;


		// 
		beam_tke.SetValue(m_beam_tke);
		frag_tke.SetValue(m_frag_tke);

	}
}
