
int main (void) {
    VbCommonParams cparams;
    VbSelectFirmwareParams fparams;
    VbNcContext vnc;

	/* Load NV storage */
	VbExNvStorageRead(vnc.raw);
	VbNvSetup(&vnc);
    
    LoadFirmware(cparams, fparams, &vnc);

}
