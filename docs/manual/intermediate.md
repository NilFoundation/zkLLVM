\page BLS BLS weighted threshold signature

BLS weighted threshold signature
=============================

In this tutorial we dive deeper into the cryptography constructs which crypto3 library implements.
By the end of this tutorial , a user will be in a position to start combining different schemes
and understand how major classes interact.


Pre-requisites
================
The user has set up their development environment as outlined in the quickstart tutorial [TODO : here]() 
and can compile example programs in the scaffold repository.


Background
===============
ECDSA signatures are fairly common in most protocols ex Bitcoin/Ethereum. BLS signatures use 
different curve/computation which enables nicer properties such as signature aggregation. 
Signature aggregation allows for multiple parties to compute a single signature. ECDSA multisigs 
leak privacy as it is easy to identify a multisig public key and transactions leak data like the 
signer or scheme ex: 3 of 5 signatures and needs to be done onchain for each signature. BLS signatures 
can be aggregated offline and 
appear no different from a traditional public key. Thus preserving privacy and reducing compute resources 
in verification. Threshold signatures take this aggregation a step further by assigning weights
to each of the signatures and only considering a signature valid when a threshold is reached.

The example we will construct now is for a BLS weighted threshold scheme.


BLS Weighted Threshold Scheme
==========









