#ifndef RefitVertex_h
#define RefitVertex_h

#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"

class RefitVertex : public reco::Vertex {

	public:
		RefitVertex();
		RefitVertex(reco::Vertex vtx);
		// Get user-defined vertex reference
		// It returns a null ref if the key is not found
		// To check if a key exists, use hasUserVtx
		reco::VertexRef userVtx(const std::string & key) const;
		// Set user-defined int
		void addUserVtx(const std::string & label, const edm::Ptr<reco::Candidate> & data, const bool overwrite = false);
		// Get list of user-defined vtx names
		const std::vector<std::string> & userVtxNames() const { return userVtxLabels_;}
		// Return true if there is a user-defined int with a given name
		bool hasUserVtx( const std::string & key) const {
			auto it = std::lower_bound(userVtxLabels_.cbegin(), userVtxLabels_.cend(), key);
			return ( it != userVtxLabels_.cend() && *it == key);
		}
		
	private:
		std::vector<std::string> userVtxLabels_;

};

//typedef std::vector<RefitVertex> RefitVertexCollection;

#endif
