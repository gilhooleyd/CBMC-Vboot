# -*- mode: ruby -*-
# vi: set ft=ruby :
Vagrant.configure(2) do |config|
  # Don't ever overwrite the box with an update
  config.vm.box_check_update = false

  # Use Fedora 23
  config.vm.box = "fedora/23-cloud-base"
  # X11 Forwarding enabled
  config.ssh.forward_x11 = true

  # We need at least 1 Gb of Memory for compiling
  config.vm.provider "virtualbox" do |vb|
    # Customize the amount of memory on the VM:
    vb.memory = "1024"
  end
end
