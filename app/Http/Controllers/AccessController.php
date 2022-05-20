<?php

namespace App\Http\Controllers;

use App\Models\AccessLog;
use App\Models\Doorlock;
use App\Models\NfcKey;
use Illuminate\Http\Request;

class AccessController extends Controller
{
    public function authenticate(Request $request) {

        // Recupero dal body della richiesta HTTP POST i valori dei campi nfc_id e secret_key che mi ha inviato l'ESP32
        $nfc_uid = $request->input('nfc_uid');
        $secret_key = $request->input('secret_key');

        // Ricerchiamo nel database se esistono un'entità NFCKey ed un'entità Doorlock rispettivamente con la NFC_UID e la SECRET_KEY ricevute in ingresso nella richiesta HTTP POST
        $nfcKey = NfcKey::where('nfc_uid', $nfc_uid)->first();
        $doorlock = Doorlock::where('secret_key', $secret_key)->first();

        // Se la NFC Key oppure la Doorlock non esistono nel Database entro in questo if e ritorno un errore con risposta HTTP Status Code 401
        if($nfcKey == null || $doorlock == null) {
            return response()->json(
                [
                    'message' => 'NFC Key or Doorlock not found'
                ], 401);
        }

        // Se trovo sia l'NFC Key e sia la Doorlock, valutiamo se sono associate tra loro
        if($nfcKey->doorlock_id == $doorlock->id) {

            // Creo un nuovo Access Log nel database
            $accessLog = new AccessLog;
            $accessLog->doorlock_id = $doorlock->id;
            $accessLog->nfckey_id = $nfcKey->id;
            $accessLog->save();

            response()->json(
                [
                    'message' => 'Door unlocked'
                ], 200);
        } else {
            return response()->json(
                [
                    'message' => 'This NFC Key is not authorized to open this Doorlockk'
                ], 401);
            }
    }

}
