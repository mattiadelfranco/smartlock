<?php

namespace App\Models;

use Illuminate\Database\Eloquent\Factories\HasFactory;
use Illuminate\Database\Eloquent\Model;

class Doorlock extends Model
{
    use HasFactory;

    protected $guarded = [];

    public function nfcKeys() {
        return $this->hasMany(NfcKey::class);
    }

    public function accessLogs() {
        return $this->hasMany(AccessLog::class);
    }
}
