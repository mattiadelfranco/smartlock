<?php

namespace App\Models;

use Illuminate\Database\Eloquent\Factories\HasFactory;
use Illuminate\Database\Eloquent\Model;

class AccessLog extends Model
{
    use HasFactory;

    protected $guarded = [];

    public function doorlock() {
        return $this->belongsTo(Doorlock::class);
    }

    public function nfcKey() {
        return $this->belongsTo(NfcKey::class);
    }
}
