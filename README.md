# Google Chrome Content Analysis Connector Agent SDK

The Google Chrome Content Analysis Connector provides an official mechanism
allowing Data Loss Prevention (DLP) agents to more deeply integrate their
services with Google Chrome.

DLP agents are background processes on managed computers that allow enterprises
to monitor locally running applications for data exfiltration events.  They can
allow/block these activities based on customer defined DLP policies.

This repository contains the SDK that DLP agents may use to become service
providers for the Google Chrome Content Analysis Connector.  The code that must
be compiled and linked into the content analysis agent is located in the agent
subdirectory.

The code that must be compiled and linked into Google Chrome is located in
the browser subdirectory.
