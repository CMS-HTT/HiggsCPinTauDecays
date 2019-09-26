# TauRefit

Package to calculate the Primary Vertex position without the tracks from tau-lepton decay products.

This package has been used with 2016 MiniAOD data and MC samples.
It may need changes for being used with new samples (2017) and for AOD data format.

**Note** Ignore all the files with naming `RefitVertexProducer*` and `NonTauPairTrackCollectionProducer*`: these are from a preliminary version of the refitting vertex package. They will be removed in future.


## How does it work?

In the following an overview of the relevant part of the code.


### `interface/RefitVertex.h`

New class which:

* Inherits from `public reco::Vertex`.
* Additional methods are defined. They are needed in order to have a hash code associated to each refitted vertex.
* Definition of the type `RefitVertexCollection`, which is a `vector<RefitVertex> `.

The latter item is indeed because at this level the signal tau-lepton pair is not known yet. Given N leptons in an event, all possible pairs of two leptons are formed, and for each pair the refitted vertex is calculated. This gives for each event a vector of refitted vertices, which are saved in the above mentioned collection.


### `plugins/AdvancedRefitVertexProducer.{h,cc}`

EDProducer to calculate the vertex position without using the tracks from tau-lepton decay products. The workflow of the producer is the following:

* Get the collections of reconstructed particles.

* Create a collection of all leptons in the event and create all possible pairs from it.

* For each pair:

	* Loop over the PF candidates (`pat::PackedCandidate`). If a match between the PF candidate and one of the pair components is found, its track is excluded. All the tracks of the PF candidates that do not match any of the pair components, are saved into a track collection.
	* The refitting of the vertex is performed by calling the `AdaptiveVertexFitter` and passing the new track collection just created. The fit can be performed with or without the beamspot used as a constraint.
	* The hash code for the refitted vertex is calculated from the pair components.
	* The refitted vertex is pushed into the vertex collection.

* The vertex collection is saved in the event.


### `python/AdvancedRefitVertexProducer_cfi.py`

Configuration of the producer. Two possible configurations, which differ in the value of the bolean variable `useBeamSpot`: this variable defines if the vertex fitter has to use the beam spot as a constraint or not.
