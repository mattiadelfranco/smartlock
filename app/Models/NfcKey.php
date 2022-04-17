<?php

namespace App\Models;

use Illuminate\Database\Eloquent\Factories\HasFactory;
use Illuminate\Database\Eloquent\Model;

class NfcKey extends Model
{
    use HasFactory;

    protected $guarded = [];

    public function doorlock() {
        return $this->belongsTo(Doorlock::class);
    }
}
