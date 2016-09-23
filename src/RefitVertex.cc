#include "VertexRefit/TauRefit/interface/RefitVertex.h"

RefitVertex::RefitVertex() {
}

RefitVertex::RefitVertex(reco::Vertex vtx) : reco::Vertex(vtx) {
}

edm::Ptr<reco::Candidate> RefitVertex::userCand( const std::string & key ) const {
	auto it = std::lower_bound(userCandLabels_.cbegin(), userCandLabels_.cend(), key);
	if (it != userCandLabels_.cend()) {
		return userCands_[std::distance(userCandLabels_.begin(),it)];
	}
	return edm::Ptr<reco::Candidate>();
}

void RefitVertex::addUserCand( const std::string & label, const edm::Ptr<reco::Candidate> & data, const bool overwrite) {
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
