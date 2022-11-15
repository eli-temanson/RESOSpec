/*
	RESOAnalysisStage.h
	Example of a user AnalysisStage. This one is based around the SE-RESO detector system in FoxLab at FSU.

	GWM -- Feb 2022
*/
#include "Specter.h"

#define _USE_MATH_DEFINES
#include <cmath>

namespace Specter {

	class RESOAnalysisStage : public AnalysisStage
	{
	public:
		RESOAnalysisStage(const SpectrumManager::Ref& manager);
		virtual ~RESOAnalysisStage();

		virtual void AnalyzePhysicsEvent(const SpecEvent& event) override;

	private:
		void CalcSiPos(Parameter&, Parameter&, const double,const double,const double,const double, const double);

		//Create a bunch of parameters
		Parameter s1_front_emax, s1_back_emax;
		Parameter s2_front_emax, s2_back_emax;
		Parameter s1_front_chmax, s1_back_chmax;
		Parameter s2_front_chmax, s2_back_chmax;
		Parameter s1_theta, s1_phi;
		Parameter s2_theta, s2_phi;
		Parameter qval;


		//For collections of parameters, prefer std::unordered_map or std::map over vector
		//Better with use of UUIDs for board-channel pairs
		std::unordered_map<uint32_t, Parameter> chmap;

		std::unordered_map<uint32_t, Parameter> adc[5];
		int adc_geo[5] = {4,11,12,13,14};
		std::unordered_map<uint32_t, Parameter> tdc[4];
		int tdc_geo[5] = {6,7,8,9};
		std::unordered_map<uint32_t, Parameter> mqdc;
		std::unordered_map<uint32_t, Parameter> mtdc;

		//Create a few variables
		// Variable x1_weight;
		// Variable x2_weight;
		Variable frag_tke;
		Variable beam_tke;

		//Create a scaler
		// Scaler beamIntegrator;

		//Define some board-channel ID's that we'll use. The static keyword means that we only need to calculate them once,
		//constexpr allows us to use them in a switch (and makes them compile time evaluated)
		//Note that to make them static constexpr we use literals (i.e. hardcoded) in the arguments
		static constexpr uint32_t s_ice_ID         = Utilities::GetBoardChannelUUID(4, 0);
		static constexpr uint32_t s_icde_ID        = Utilities::GetBoardChannelUUID(4, 0);
		static constexpr uint32_t s_tb_master_ID   = Utilities::GetBoardChannelUUID(18, 16);
		static constexpr uint32_t s_tb_icds_ID     = Utilities::GetBoardChannelUUID(18, 17);
		static constexpr uint32_t s_tb_neutor_ID   = Utilities::GetBoardChannelUUID(18, 18);
		static constexpr uint32_t s_tb_s2_ID       = Utilities::GetBoardChannelUUID(18, 19);
		static constexpr uint32_t s_tb_s1_ID       = Utilities::GetBoardChannelUUID(18, 20);
		static constexpr uint32_t s_tb_rf_ID       = Utilities::GetBoardChannelUUID(18, 21);

		static constexpr double s_tdc_conv = 0.0625; // mesytec ch to ns conversion, defined in the nscldaq config

	};

}