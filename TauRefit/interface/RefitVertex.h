#ifndef RefitVertex_h
#define RefitVertex_h

#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/Candidate/interface/Candidate.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

class RefitVertex : public reco::Vertex {

	public:
		RefitVertex() {};
		RefitVertex( reco::Vertex vtx ) : reco::Vertex(vtx) {};

		// Get user-defined candidate reference
		// It returns a null ref if the key is not found
		// To check if a key exists, use hasUserCand
		edm::Ptr<reco::Candidate> userCand( const std::string & key ) const {
			auto it = std::lower_bound(userCandLabels_.cbegin(), userCandLabels_.cend(), key);
			if (it != userCandLabels_.cend()) {
				return userCands_[std::distance(userCandLabels_.begin(),it)];
			}
			return edm::Ptr<reco::Candidate>();
		}

		// Set user-defined int
		void addUserCand( const std::string & label, const edm::Ptr<reco::Candidate> & data, const bool overwrite = false ) {
			auto it = std::lower_bound(userCandLabels_.begin(), userCandLabels_.end(), label);
			const auto dist = std::distance(userCandLabels_.begin(), it);
			if( it == userCandLabels_.end() || *it != label ) {
				userCandLabels_.insert(it, label);
				userCands_.insert(userCands_.begin()+dist,data);
			} else if( overwrite && *it == label ) {
				userCands_[dist] = data;
			} else {
				edm::LogWarning("addUserCand") << "Attempting to add userCand " << label << " failed, Ptr exists already!";
			}
		}
		
		// Get list of user-defined candidate names
		const std::vector<std::string> & userCandNames() const { return userCandLabels_; }
		
		// Return true if there is a user-defined int with a given name
		bool hasUserCand( const std::string & key ) const {
			auto it = std::lower_bound(userCandLabels_.cbegin(), userCandLabels_.cend(), key);
			return ( it != userCandLabels_.cend() && *it == key);
		}
		
	private:
		std::vector<std::string> userCandLabels_;
		std::vector<edm::Ptr<reco::Candidate> > userCands_;

};

typedef std::vector<RefitVertex> RefitVertexCollection;

#endif
