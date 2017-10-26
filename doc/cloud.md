# Getting Started with GCP for Shallow Waters

There is no one-click start button for Google Compute Platform, but we
have tried to make these instructions as painless as possible.  Note
that you must *read and follow these instructions thoroughly*. There
are a number of small deviations you can make that will be difficult
to debug without prior experience with GCP.

## Account Set-up

The following instructions cover basic account set-up instructions.

* The first thing you want to do is log into GCP. Go to 
  [the GCP website](https://cloud.google.com/) and sign in with your cmail
  account.  You should already have received an email with
  instructions about getting a coupon.
* After you get a coupon, you can add it to a Billing Account.  Call
  the billing account *CS5220-billing*. The coupon is good for 50
  dollars, which should be more than enough assuming responsible use.
* Next, create a GCP project. Each GCP project needs to be linked to a
  billing account. Create a project named *CS5220-project* and use
  *CS5220-billing* as the account.
* At this point, you can spin up a virtual machine instance, but you
  to request a quota increase for GPUs in order to get one on an
  instance. The particular path of buttons you want to click is:
	* IAM and Admin
	* Quotas
	* Nvidia K80 GPU (us-east1)
	* Edit Quotas 
	* Change current limit to 1 (reason, "GPU needed for Cornell CS5220 Class Project")
	* Submit Request
  The quota should be approved immediately via email. 

## Spinning Up an Instance

Now, we cover how to properly spin up and configure a GCP instance. 

* Go to Compute Engine -> VM Instances -> Create. You should then
  enter a page with a number of instance configuration options. These
  are important!
* Name the instance *CS5220-instance*
* The Zone should be *us-east1-[a,b,c,d]*, as your GPU as been
  specifically requested for that region. You can pick *us-east1-a*,
  *us-east1-b*, *us-east1-c*, or *us-east1-d*.
* Under Machine Type, click customize. You will want
	* 1 vCPU (default)
	* 3.75 gigabytes of RAM (default)
	* 1 Nvidia Tesla K80
  As a sanity check, the hourly billing rate should be about 70 cents,
  giving one about 70 hours of development time with a 50 dollar
  coupon
* Check the boxes *Allow HTTP traffic* and *Allow HTTPS traffic*
* Under Boot Disk, choose *Ubuntu 16.04*
* Click *Create*

Note that you may want to create a non-GPU instance with the same
software setup as a base for software development, as it is much less
expensive to run a non-GPU instance.

## Connecting to the Instance

After your virtual machine instance has spun up, you will need to
connect to it.

* You can ssh in via GCP's own interface, by clicking the *SSH* button
  under *Connect*
* You can also ssh in via the command line; see the instructions
  [here](https://cloud.google.com/compute/docs/instances/connecting-to-instance).
* To transfer files between the instance and your machine, see the
  instructions
  [here](https://cloud.google.com/compute/docs/instances/transfer-files). 
  For code, you may choose to just use git.

## Configuring the Instance

The VM that you have just spun up is almost completely blank; you will
need to install a number of things yourself. Fortunately, python and
git are already installed.

* The first thing you will want to do is clone this project onto your VM. 
* If you haven't already noticed, we have provided a shell script that will install everything you need automatically. 
To run it, type in **chmod a+x configure-vm.sh; sudo ./configure-vm.sh**. You will have to wait for a bit as everything installs (note that you will have to interact with the shell script by pressing enter/yes a few times). 
* Log out and log back in. CUDA, Make, and the SciPy stack should be installed. 
* To check the CUDA installation, type in **nvidia-smi**, which should output
information about the GPU on your machine (the GPU should be a Tesla K80)
* To check the Make and SciPy installations, simply type in **make lshallow** in the c or c++ source directories. Copy the executable up a level, and then type **make wave.out**. This will run the shallow waters code and output
a video file. 
* When you are not using the instance, make sure to stop it so that you are not billed for time that you are not using. THIS IS VERY IMPORTANT. To do so, check the box next to *CS5220-instance* and click the stop button. You can restart the instance whenever you are ready to use it. 

## Completely Cleaning Up

To completely clean up, you will want to
* Delete all instances you are using
* Delete the project you are using

Otherwise, you will be continually billed a project management fee
until your coupon money runs out!
