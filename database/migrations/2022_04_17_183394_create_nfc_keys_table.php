<?php

use Illuminate\Database\Migrations\Migration;
use Illuminate\Database\Schema\Blueprint;
use Illuminate\Support\Facades\Schema;

return new class extends Migration
{
    /**
     * Run the migrations.
     *
     * @return void
     */
    public function up()
    {
        Schema::create('nfc_keys', function (Blueprint $table) {
            $table->id();
            $table->timestamps();
            $table->boolean('enabled')->default(true);
            $table->string('nfc_uid');
            $table->foreignId('doorlock_id');
        });
    }

    /**
     * Reverse the migrations.
     *
     * @return void
     */
    public function down()
    {
        Schema::dropIfExists('nfckeys');
    }
};
