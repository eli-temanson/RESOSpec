/*
	RESOAnalysisStage.cpp
	Example of a user AnalysisStage. This one is based around the SE-RESO detector system in FoxLab at FSU.

	GWM -- Feb 2022
*/
#include "RESOAnalysisStage.h"

namespace Specter 
{
	//Construct each NavParameter with their unique name. Then bind them to the SpectrumManager.
	RESOAnalysisStage::RESOAnalysisStage(const SpectrumManager::Ref& manager) :
		AnalysisStage("RESOAnalysis"), 
		s1_front_emax("s1_front_emax"),
		s1_back_emax("s1_back_emax"),
		s2_front_emax("s2_front_emax"),
		s2_back_emax("s1_back_emax"),
		s1_front_chmax("s1_front_chmax"),
		s1_back_chmax("s1_back_chmax"),
		s2_front_chmax("s2_front_chmax"),
		s2_back_chmax("s1_back_chmax"),
		s1_theta("s1_theta"),
		s1_phi("s1_phi"),
		s2_theta("s2_theta"),
		s2_phi("s2_phi"),
		frag_tke("frag_tke"),
		qval("qval")
	{
		SPEC_PROFILE_FUNCTION();
		//Bind some parameters
		manager->BindParameter(s1_front_emax);
		manager->BindParameter(s1_back_emax);
		manager->BindParameter(s2_front_emax);
		manager->BindParameter(s2_back_emax);
		manager->BindParameter(s1_front_chmax);
		manager->BindParameter(s1_back_chmax);
		manager->BindParameter(s2_front_chmax);
		manager->BindParameter(s2_back_chmax);
		
		manager->BindVariable(frag_tke);

		//Bind parameters with some default histograms. Saves us the effort of making them in the UI.
		// manager->BindParameter(x1, 600, -300.0, 300.0);

		//Example of injecting experiment specific info. I know that SABRE is only used in 16 channel digitizers,
		//so I can make a simple incremental id for use with the UI so that the parameter names are human readable. We map this "simple" id
		//to the more robust and universal Specter board/channel UUID using a std::unordered_map
		//This is kind of a really simple example of injecting a channel map; more complicated and advanced versions could involve reading
		//a text file to get detector ID information, detector channel number, etc.
		std::vector<std::string> adc_list[5]; //list of names will allow us to create a summary histogram.
		std::vector<std::string> tdc_list[4]; //list of names will allow us to create a summary histogram.
		std::vector<std::string> mqdc_list; //list of names will allow us to create a summary histogram.
		std::vector<std::string> mtdc_list; //list of names will allow us to create a summary histogram.
		
		uint32_t uuid;
		uint32_t index = 0; // start at index 1
		for (auto& board : adc)
		{
			for (uint32_t channel = 0; channel < 32; channel++)
			{
				//spdlog comes prepackaged with the fmt library, so make good use of it!
				adc_list[index].push_back(fmt::format("adc_{}_{}",index,channel));
				uuid = Utilities::GetBoardChannelUUID(adc_geo[index], channel);
				chmap[uuid] = Parameter(adc_list[index].back()); 
				manager->BindParameter(board[uuid]);
			}
			++index;
		}
		index = 0; // start at index 1
		for (auto& board : tdc)
		{
			for (uint32_t channel = 0; channel < 32; channel++)
			{
				//spdlog comes prepackaged with the fmt library, so make good use of it!
				tdc_list[index].push_back(fmt::format("tdc_{}_{}",index,channel));
				uuid = Utilities::GetBoardChannelUUID(tdc_geo[index], channel);
				chmap[uuid] = Parameter(tdc_list[index].back());
				manager->BindParameter(board[uuid]);
			}
			++index;
		}
		for (uint32_t channel = 0; channel < 32; channel++)
		{
			//spdlog comes prepackaged with the fmt library, so make good use of it!
			mtdc_list.push_back(fmt::format("mtdc_{}",channel));
			uuid = Utilities::GetBoardChannelUUID(18, channel);
			chmap[uuid] = Parameter(mtdc_list.back());
			manager->BindParameter(mtdc[uuid]);
		}
		for (uint32_t channel = 0; channel < 32; channel++)
		{
			//spdlog comes prepackaged with the fmt library, so make good use of it!
			mqdc_list.push_back(fmt::format("mqdc_{}",channel));
			uuid = Utilities::GetBoardChannelUUID(20, channel);
			chmap[uuid] = Parameter(mqdc_list.back());
			manager->BindParameter(mqdc[uuid]);
		}


		//If you want to make a histogram default available, you can add one like this.
		// manager->AddHistogramSummary(HistogramArgs("sabre_summary", "", 512, 0.0, 16384), sabre_list);
		manager->AddHistogramSummary(HistogramArgs("summary_adc0", "", 1024, 0.0, 4096), adc_list[0]);
		manager->AddHistogramSummary(HistogramArgs("summary_adc1", "", 1024, 0.0, 4096), adc_list[1]);
		manager->AddHistogramSummary(HistogramArgs("summary_adc2", "", 1024, 0.0, 4096), adc_list[2]);
		manager->AddHistogramSummary(HistogramArgs("summary_adc3", "", 1024, 0.0, 4096), adc_list[3]);
		manager->AddHistogramSummary(HistogramArgs("summary_adc4", "", 1024, 0.0, 4096), adc_list[4]);
		manager->AddHistogramSummary(HistogramArgs("summary_tdc0", "", 1024, 0.0, 4096), tdc_list[0]);
		manager->AddHistogramSummary(HistogramArgs("summary_tdc1", "", 1024, 0.0, 4096), tdc_list[1]);
		manager->AddHistogramSummary(HistogramArgs("summary_tdc2", "", 1024, 0.0, 4096), tdc_list[2]);
		manager->AddHistogramSummary(HistogramArgs("summary_tdc3", "", 1024, 0.0, 4096), tdc_list[3]);
		manager->AddHistogramSummary(HistogramArgs("summary_mtdc", "", 1024, 0.0, 4096), mtdc_list);
		manager->AddHistogramSummary(HistogramArgs("summary_mqdc", "", 1024, 0.0, 4096), mqdc_list);

		manager->AddHistogram(HistogramArgs("ic_ede_raw","adc_0_0","adc_0_1",512,0,4096,512,0,4096));
		manager->AddHistogram(HistogramArgs("si_ede_raw","s1_back_emax","s2_back_emax",512,0,4096,512,0,4096));
		manager->AddHistogram(HistogramArgs("s2_kin_raw","s2_front_chmax","s2_back_emax",16,0,16,512,0,4096));
		manager->AddHistogram(HistogramArgs("s1_kin_raw","s1_front_chmax","s1_back_emax",16,0,16,512,0,4096));

		manager->AddHistogram(HistogramArgs("trig_bit_master","mtdc_16",512,0,4096));
		manager->AddHistogram(HistogramArgs("trig_bit_ic_ds","mtdc_17",512,0,4096));
		manager->AddHistogram(HistogramArgs("trig_bit_neut","mtdc_18",512,0,4096));
		manager->AddHistogram(HistogramArgs("trig_bit_s2","mtdc_19",512,0,4096));
		manager->AddHistogram(HistogramArgs("trig_bit_s1","mtdc_20",512,0,4096));
		manager->AddHistogram(HistogramArgs("trig_bit_rf","mtdc_21",512,0,4096));


		//Note that if you save a config, the config rep of this histogram will supercede this version.

		// manager->BindVariable(x1_weight);
		// manager->BindVariable(x2_weight);

		// manager->BindScaler(beamIntegrator);
	}

	RESOAnalysisStage::~RESOAnalysisStage() {}

	//Do some physics!
	void RESOAnalysisStage::AnalyzePhysicsEvent(const SpecEvent& event)
	{
		SPEC_PROFILE_FUNCTION();
		//You might want some flags for coincidence cases
		//Use statics to avoid allocating extra memory each call (these pipeline functions are called a lot!)
		static bool foundFlag = false;

		//Most analysis stages will start kinda like this. Take the raw event data and
		//put it into Specter::Parameters using the hit id. Switches are perfect for this. Can also
		//create mapping classes to use text-file-based id association (commonly called channel maps).
		for(auto& hit : event)
		{
			foundFlag = false;

			auto iter = chmap.find(hit.id);
			if (iter != chmap.end())
			{
				iter->second.SetValue(hit.longEnergy);
				foundFlag = true;
			}
		}


		// loop over silicon channels to find the max energy and ch.
		for(uint32_t i=0;i<32;i++)
		{
			auto& mtdc_param = chmap[Utilities::GetBoardChannelUUID(18,i)];
			auto& mqdc_param = chmap[Utilities::GetBoardChannelUUID(20,i)];
			auto& s1_param = chmap[Utilities::GetBoardChannelUUID(14,i)];
			auto& s2_param = chmap[Utilities::GetBoardChannelUUID(13,i)];

			// convert mtdc time
			if(mtdc_param.IsValid())
			{
				mtdc_param.SetValue(mtdc_param.GetValue()*s_tdc_conv);
			}
			// channels 0-15
			if(i < 16)
			{
				if(s1_param.IsValid()) // s1.front
				{
					if(s1_front_emax.GetValue() < s1_param.GetValue())
					{
						s1_front_emax.SetValue(s1_param.GetValue());
						s1_front_chmax.SetValue((double)i);
					}	
				}
				if(s2_param.IsValid()) // s2.front
				{
					if(s2_front_emax.GetValue() < s2_param.GetValue())
					{
						s2_front_emax.SetValue(s2_param.GetValue());
						s2_front_chmax.SetValue((double)i);
					}	
				}
				if(mtdc_param.IsValid() && mqdc_param.IsValid()) // neutron
				{
					if(s2_front_emax.GetValue() < mtdc_param.GetValue())
					{
						s2_front_emax.SetValue(s2_param.GetValue());
						s2_front_chmax.SetValue((double)i);
					}	
				}

			// channels 16-31
			}else{
				if(s1_param.IsValid()) // s1.back
				{
					if(s1_back_emax.GetValue() < s1_param.GetValue())
					{
						s1_back_emax.SetValue(s1_param.GetValue());
						s1_back_chmax.SetValue((double)i);
					}	
				}
				if(s2_param.IsValid()) // s2.back
				{
					if(s2_front_emax.GetValue() < s2_param.GetValue())
					{
						s2_back_emax.SetValue(s2_param.GetValue());
						s2_back_chmax.SetValue((double)i);
					}	
				}
			}


		} // end of 32 index loop 


		// Set energy calibration


		// Get Silicon position
		CalcSiPos(s1_theta, s1_phi, s1_front_chmax.GetValue(), s1_back_chmax.GetValue(), 24.0, 48.0, 124.4); // units in [mm]
		CalcSiPos(s2_theta, s2_phi, s2_front_chmax.GetValue(), s2_back_chmax.GetValue(), 11.53, 35.0, 68.9);

		// Calculate decay q value


	}

	void RESOAnalysisStage::CalcSiPos(Parameter &theta, Parameter &phi, const double fch, const double bch, const double inrad, const double outrad, const double z)
	{
		double x = bch*2*M_PI/16.0;
		double y = inrad + (outrad - inrad)/16.0 * fch;
		double r = std::hypot(x,y);

		theta.SetValue(std::acos(r/z));
		phi.SetValue(std::atan2(y,x));
	}

	void RESOAnalysisStage::CalcDecayQValueEst(Parameter& q, ){
		double a = Decay_Light.KE*(1 + Decay_Light.M/Decay_Heavy.M);
		double b = FragEst*(1 - Fragment.M/Decay_Heavy.M);
		double c = 2.0/Decay_Heavy.M;
		double d = std::sqrt(Decay_Light.KE * Decay_Light.M * FragEst * Fragment.M); 
		DecayQValueEst = a - b - c*d*std::cos( Decay_Light.Theta );
	}



}